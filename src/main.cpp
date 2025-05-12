#include "FlowGuard/flowguard.h"
#include "FlowGuard/logger.h"
#include <termios.h>
#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <algorithm>  

// Run the ML model script with the flow value
std::string runMLModel(float flow_value) {
    std::ostringstream cmd;
    cmd << "python3 ml/ml_model.py " << flow_value;

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

// Try to extract float value from something like "FLOW:52.3"
float parseFlowValueFrom(const std::string& str) {
    float value = 0.0;
    size_t pos = str.find("FLOW:");
    if (pos != std::string::npos) {
        try {
            value = std::stof(str.substr(pos + 5));
        } catch (...) {
            // ignore
        }
    }
    return value;
}

int main() {
    // Logger başlatılıyor
    flowguard::Logger::getInstance().setLogFile("flowguard.log");
    flowguard::Logger::getInstance().log(flowguard::LogLevel::INFO, "Logger initialized");

    // Seri port açılıyor
    flowguard::SerialComm comm("/dev/ttyUSB0", B9600);

    if (!comm.openPort()) {
        flowguard::Logger::getInstance().log(flowguard::LogLevel::ERROR, "Failed to open serial port.");
        return 1;
    }

    comm.makeNonBlocking();

    comm.startTimeoutMonitor();

    while (true) {
        auto data = comm.readBytes(64);

        if (!data.empty()) {
            std::cout << "[FlowGuard] Received: ";
            for (uint8_t byte : data) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
            }
            std::cout << std::endl;

            std::string asciiStr(data.begin(), data.end());
            flowguard::Logger::getInstance().log(flowguard::LogLevel::INFO, "ASCII: " + asciiStr);

            float flowValue = parseFlowValueFrom(asciiStr);
            std::string verdict = runMLModel(flowValue);
            std::cout << "[ML] Flow " << flowValue << " is " << verdict << std::endl;

            flowguard::Logger::getInstance().log(flowguard::LogLevel::INFO, "ML verdict: " + verdict);
        } else {
            comm.reconnectIfNeeded();
        }
  
      usleep(100000); 
  }
  


      

      return 0;
    }

    

