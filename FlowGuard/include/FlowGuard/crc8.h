#pragma once
#include <vector>
#include <cstdint>

namespace flowguard {
    class CRC8 {
    public:
        static uint8_t calculate(const std::vector<uint8_t>& data);
    };
}
