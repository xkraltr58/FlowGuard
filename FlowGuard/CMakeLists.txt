cmake_minimum_required(VERSION 3.10)
project(flowguard)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

# Ana uygulama
add_executable(flowguard
    src/main.cpp
    src/flowguard.cpp
    src/logger.cpp
    src/utils.cpp
    src/crc8.cpp
)

# Test uygulaması (AYRI executable)
add_executable(test_crc8
    test_crc8.cpp
    src/crc8.cpp  # CRC hesaplama burada da lazım
)

# Her iki hedef için include klasörü
target_include_directories(flowguard PRIVATE include)
target_include_directories(test_crc8 PRIVATE include)
