#ifndef FLOWGUARD_
#define FLOWGUARD_

#include <string>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>

namespace flowguard {

  class SerialComm {
    public:
      SerialComm(const std::string& port_name, int baudrate);
      ~SerialComm();

      bool openPort();
      void closePort();
      bool isOpen() const;

      std::vector<uint8_t> readBytes(size_t num_bytes);
      void makeNonBlocking();

      void startTimeoutMonitor();  
      void stopTimeoutMonitor();
      void reconnectIfNeeded(); 
      
    private:
      int fd_;
      std::string port_name_;
      int baudrate_;
      bool is_open_;

      std::atomic<bool> monitoring_;
      std::thread timeout_thread_;
      std::chrono::steady_clock::time_point last_read_time_;  

      bool configurePort();
      void monitorTimeout(); 
      
      

  };
}

#endif //FLOWGUARD_