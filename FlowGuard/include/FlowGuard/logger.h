#ifndef FLOWGUARD_LOGGER_H
#define FLOWGUARD_LOGGER_H

#include <string>
#include <fstream>
#include <mutex>
#include <memory>

namespace flowguard {

enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    DEBUG
};

class Logger {
public:
    static Logger& getInstance();

    
    void setLogFile(const std::string& directory);

    void enableConsoleOutput(bool enable);

    
    void log(LogLevel level, const std::string& message);

private:
    Logger(); 
    ~Logger();

    std::string getCurrentTimestamp() const;
    std::string logLevelToString(LogLevel level) const;

    std::ofstream log_file_;
    std::mutex log_mutex_;
    bool console_output_enabled_;
};

} // namespace flowguard

#endif // FLOWGUARD_LOGGER_H
