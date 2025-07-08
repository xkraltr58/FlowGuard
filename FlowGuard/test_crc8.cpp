#include "FlowGuard/crc8.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

int main() {
    std::string input;
    std::cout << "Bir metin girin (örnek: FLOW:25.6): ";
    std::getline(std::cin, input);

    std::vector<uint8_t> bytes(input.begin(), input.end());
    uint8_t crc = flowguard::CRC8::calculate(bytes);

    std::cout << "CRC8 (hex): 0x" << std::hex << std::uppercase << std::setw(2)
              << std::setfill('0') << (int)crc << std::endl;

    return 0;
}
