#include "FlowGuard/logger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <ctime>

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

void Logger::setLogFile(const std::string& file_path) {
    std::lock_guard<std::mutex> lock(log_mutex_);
    log_file_.open(file_path, std::ios::app);
    if (!log_file_.is_open()) {
        std::cerr << "[Logger] Failed to open log file: " << file_path << std::endl;
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
    std::string log_entry = "[" + timestamp + "] [" + level_str + "] " + message;

    if (log_file_.is_open()) {
        log_file_ << log_entry << std::endl;
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
