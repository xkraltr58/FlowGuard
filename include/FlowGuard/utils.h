#ifndef FLOWGUARD_UTILS_H
#define FLOWGUARD_UTILS_H

#include <string>
#include <chrono>

namespace flowguard {

class Utils {
public:
    // Returns current time as a formatted string
    static std::string currentTimestamp();

    // Trims leading and trailing whitespace from a string
    static std::string trim(const std::string& str);

    // Converts errno into a readable string
    static std::string getLastErrorString();
};

} 

#endif // FLOWGUARD_UTILS_H
