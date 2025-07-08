#include "FlowGuard/utils.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <iostream>

namespace flowguard {

std::string Utils::currentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::tm buf;
    localtime_r(&time_t_now, &buf);

    std::ostringstream oss;
    oss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Utils::trim(const std::string& str) {
    const auto begin = str.find_first_not_of(" \t\n\r");
    const auto end = str.find_last_not_of(" \t\n\r");
    if (begin == std::string::npos || end == std::string::npos)
        return "";
    return str.substr(begin, end - begin + 1);
}

std::string Utils::getLastErrorString() {
    return std::string(strerror(errno));
}

void Utils::logFlow(float val) {
    std::string timestamp = Utils::currentTimestamp();
    std::ofstream file("/home/naci/FlowGuard/ml/data/flow_log.csv", std::ios::app);

    if (file.is_open()) {
        file << val << "," << timestamp << std::endl;
        file.close();

        std::cout<< "Saved: " << val << " | " << timestamp << std::endl;  
    }
}



}
