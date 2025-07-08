#include "FlowGuard/flowguard.h"
#include "FlowGuard/crc8.h"
#include "FlowGuard/logger.h"
#include "FlowGuard/utils.h"
#include <termios.h>
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cmath>

float extractFlow(const std::vector<uint8_t>& data) {
    for (size_t i = 0; i + 3 < data.size(); ++i) {
        float val;
        std::memcpy(&val, &data[i], sizeof(float));

        std::cout << "Chunk[" << i << "] = " << val << std::endl;

        if (val > 25.0f && val < 60.0f && val != 0.000174522f) {
            std::cout << "[✔] Valid flow value found: " << val << std::endl;
            return val;
        }
    }

    std::cout << "[✘] No valid flow found.\n";
    return -1.0f;
}

std::string runMLModel(float flow_value) {
    std::ostringstream cmd;
    cmd << "python3 /home/naci/FlowGuard/ml/ml_model.py " << flow_value;

    FILE* pipe = popen(cmd.str().c_str(), "r");
    if (!pipe) return "error";

    char buffer[128];
    std::string result;
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);

    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result;
}

bool verifyCRC8(const std::string& data_with_crc) {
    size_t asterisk_pos = data_with_crc.find('*');
    if (asterisk_pos == std::string::npos || asterisk_pos + 2 >= data_with_crc.size())
        return false;

    std::string payload = data_with_crc.substr(0, asterisk_pos);
    std::string crc_str = data_with_crc.substr(asterisk_pos + 1, 2);

    uint8_t received_crc = static_cast<uint8_t>(std::stoi(crc_str, nullptr, 16));
    std::vector<uint8_t> payload_bytes(payload.begin(), payload.end());
    uint8_t calculated_crc = flowguard::CRC8::calculate(payload_bytes);

    return received_crc == calculated_crc;
}

int main() {
    flowguard::Logger::getInstance().setLogFile("/home/naci/FlowGuard/logger");
    flowguard::Logger::getInstance().log(flowguard::LogLevel::INFO, "Logger initialized");

    flowguard::SerialComm comm("/dev/ttyUSB0", B9600);
    if (!comm.openPort()) {
        flowguard::Logger::getInstance().log(flowguard::LogLevel::ERROR, "Failed to open serial port.");
        return 1;
    }

    comm.makeNonBlocking();
    comm.startTimeoutMonitor();

    static float previous_val = -9999.0f;

    while (true) {
        auto data = comm.readBytes(64);

        if (!data.empty()) {
            std::cout << "[FlowGuard] Received: ";
            for (uint8_t byte : data) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
            }
            std::cout << std::endl;

            

            float flowValue = extractFlow(data);
            if (flowValue < 0) {
                flowguard::Logger::getInstance().log(flowguard::LogLevel::WARNING, "No valid flow found in data.");
                continue;
            }

            
            if (std::fabs(flowValue - previous_val) > 0.05f) {
                previous_val = flowValue;
                flowguard::Utils::logFlow(flowValue);

                std::string verdict = runMLModel(flowValue);
                std::cout << "[ML] Flow " << flowValue << " is " << verdict << std::endl;

                
                size_t delim_pos = verdict.find('|');
                std::string status = (delim_pos != std::string::npos) ? verdict.substr(0, delim_pos) : "unknown";
                std::string confidence = (delim_pos != std::string::npos) ? verdict.substr(delim_pos + 1) : "N/A";

                std::ostringstream log_stream;
                log_stream << "{\"flow_value\":" << flowValue
                           << ", \"status\":\"" << status
                           << "\", \"confidence\":" << confidence << "}";

                flowguard::Logger::getInstance().log(flowguard::LogLevel::INFO, log_stream.str());
            }
        } else {
            comm.reconnectIfNeeded();
        }

        usleep(100000);
    }

    return 0;
}
