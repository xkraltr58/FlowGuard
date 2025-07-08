#ifndef FLOWGUARD_UTILS_H
#define FLOWGUARD_UTILS_H

#include <string>
#include <chrono>

namespace flowguard {

class Utils {
public:
    
    static std::string currentTimestamp();

    
    static std::string trim(const std::string& str);

    
    static std::string getLastErrorString();

    
    static void logFlow(float val);
};

} // namespace flowguard

#endif // FLOWGUARD_UTILS_H
