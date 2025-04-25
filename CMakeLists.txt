cmake_minimum_required(VERSION 3.10)
project(flowguard)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(flowguard
    src/main.cpp
    src/flowguard.cpp
    src/logger.cpp
    src/utils.cpp
    src/crc8.cpp
)
