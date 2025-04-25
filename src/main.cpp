#include "FlowGuard/flowguard.h"
#include "FlowGuard/logger.h"
#include <termios.h>
#include <iostream>
#include <unistd.h>
#include <iomanip>



int main() {
    // Logger başlatılıyor
    flowguard::Logger::getInstance().setLogFile("flowguard.log");
    flowguard::Logger::getInstance().log(flowguard::LogLevel::INFO, "Logger initialized");

    // Seri port açılıyor
    flowguard::SerialComm comm("/dev/ttyUSB0", B9600);

    if (!comm.openPort()) {
        flowguard::Logger::getInstance().log(flowguard::LogLevel::ERROR, "Failed to open serial port.");
        return 1;
    }

    comm.makeNonBlocking();

    comm.startTimeoutMonitor();

    while (true) {
      auto data = comm.readBytes(64);
  
      if (!data.empty()) {
          std::cout << "[FlowGuard] Received: ";
          for (uint8_t byte : data) {
              std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
          }
          std::cout << std::endl;
  
          std::string asciiStr(data.begin(), data.end());
          flowguard::Logger::getInstance().log(flowguard::LogLevel::INFO, "ASCII: " + asciiStr);
      } else {
          comm.reconnectIfNeeded();  // 👈 only try reconnect when there's no data
      }
  
      usleep(100000); // 👈 still keep your sleep
  }
  


      usleep(100000); // 100ms

      return 0;
    }

    

