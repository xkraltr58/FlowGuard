#include "FlowGuard/logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <algorithm>

namespace flowguard {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger()
    : console_output_enabled_(true) {}

Logger::~Logger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void Logger::setLogFile(const std::string& directory) {
    std::lock_guard<std::mutex> lock(log_mutex_);

    
    std::string timestamp = getCurrentTimestamp();  
    std::string filename = "flowguard_log_" + timestamp + ".json";

    
    std::replace(filename.begin(), filename.end(), ':', '-');
    std::replace(filename.begin(), filename.end(), ' ', '_');

    std::string full_path = directory + "/" + filename;
    log_file_.open(full_path, std::ios::app);

    if (!log_file_.is_open()) {
        std::cerr << "[Logger] Failed to open log file: " << full_path << std::endl;
    }
}


void Logger::enableConsoleOutput(bool enable) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    console_output_enabled_ = enable;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    std::string timestamp = getCurrentTimestamp();
    std::string level_str = logLevelToString(level);

    // JSON format
    std::ostringstream json;
    json << "{";
    json << "\"timestamp\":\"" << timestamp << "\",";
    json << "\"level\":\"" << level_str << "\",";
    json << "\"message\":\"" << message << "\"";
    json << "}";

    std::string log_entry = json.str();

    if (log_file_.is_open()) {
        log_file_ << log_entry << std::endl;
        log_file_.flush();
    }

    if (console_output_enabled_) {
        std::cerr << log_entry << std::endl;
    }
      
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto local_time = std::localtime(&time_t_now);

    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::logLevelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::INFO: return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}






} // namespace flowguard
