#include "FlowGuard/flowguard.h"
#include "FlowGuard/logger.h"
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

namespace flowguard {

  SerialComm::SerialComm(const std::string& port_name, int baudrate)
    : port_name_(port_name), baudrate_(baudrate), fd_(-1), is_open_(false),
      monitoring_(false), last_read_time_(std::chrono::steady_clock::now()) 
  {}

  SerialComm::~SerialComm(){
    stopTimeoutMonitor();
    closePort();
  }

  bool SerialComm::openPort() {
    fd_ = open(port_name_.c_str(), O_RDWR | O_NOCTTY);
    if (fd_ < 0) {
        flowguard::Logger::getInstance().log(LogLevel::ERROR, "[FlowGuard] Failed to open port " + port_name_ + ": " + strerror(errno));
        return false;
    }

    flowguard::Logger::getInstance().log(LogLevel::INFO, "[FlowGuard] Serial port opened successfully.");

    if (!configurePort()) {
        std::cerr << "[FlowGuard] Failed to configure port." << std::endl;
        close(fd_);
        fd_ = -1;
        return false;
    }

    flowguard::Logger::getInstance().log(LogLevel::INFO, "[FlowGuard] Serial port configured and ready.");
    is_open_ = true;
    return true;
}


  void SerialComm::closePort(){
    if(fd_ >= 0) {
      close(fd_);
      fd_ = -1;

    }
    is_open_=false;
  }

  bool SerialComm::isOpen() const {
    return is_open_;
  }

  void SerialComm::makeNonBlocking() {
    if (fd_ >= 0) {
        int flags = fcntl(fd_, F_GETFL, 0);
        if (flags != -1) {
            fcntl(fd_, F_SETFL, flags | O_NONBLOCK);
        } else {
            std::cerr << "[FlowGuard] Failed to get port flags: " << strerror(errno) << std::endl;
        }
    }
}

bool SerialComm::configurePort() {
    struct termios tty;
    if (tcgetattr(fd_, &tty) != 0) {
        std::cerr << "[FlowGuard] Error from tcgetattr: " << strerror(errno) << std::endl;
        return false;
    }

    cfsetospeed(&tty, baudrate_);
    cfsetispeed(&tty, baudrate_);

    tty.c_cflag |= (CLOCAL | CREAD);    
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 
    tty.c_cflag &= ~PARENB;             
    tty.c_cflag &= ~CSTOPB;             
    tty.c_cflag &= ~CRTSCTS;            

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); 
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);         
    tty.c_oflag &= ~OPOST;                          

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10; 

    if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
        std::cerr << "[FlowGuard] Error from tcsetattr: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

std::vector<uint8_t> SerialComm::readBytes(size_t num_bytes) {
    std::vector<uint8_t> buffer(num_bytes);
    ssize_t n = read(fd_, buffer.data(), num_bytes);
    if (n > 0) {
        buffer.resize(n);
        last_read_time_ = std::chrono::steady_clock::now();
    } else {
        buffer.clear();
    }
    return buffer;
}

void SerialComm::startTimeoutMonitor() {
  monitoring_ = true;
  timeout_thread_ = std::thread(&SerialComm::monitorTimeout, this);
}

void SerialComm::stopTimeoutMonitor() {
  monitoring_ = false;
  if (timeout_thread_.joinable()) {
      timeout_thread_.join();
  }
}

void SerialComm::monitorTimeout() {
  using namespace std::chrono;
  const milliseconds TIMEOUT_MS(1000);

  while (monitoring_) {
      std::this_thread::sleep_for(milliseconds(200)); // 200ms

      auto now = steady_clock::now();
      auto elapsed = duration_cast<milliseconds>(now - last_read_time_);

      if (elapsed > TIMEOUT_MS) {
          flowguard::Logger::getInstance().log(LogLevel::WARNING, "[FlowGuard] No data received in the last 1000 ms");
      }
  }
}

void SerialComm::reconnectIfNeeded() {
  if (!isOpen()) {
     
      if (access(port_name_.c_str(), F_OK) == 0) {
          flowguard::Logger::getInstance().log(LogLevel::INFO, "[FlowGuard] Device is available. Attempting to reconnect...");
          if (openPort()) {
              flowguard::Logger::getInstance().log(LogLevel::INFO, "[FlowGuard] Reconnected to serial port.");
          } else {
              flowguard::Logger::getInstance().log(LogLevel::ERROR, "[FlowGuard] Reconnection attempt failed.");
          }
      }
  }
}



} 
