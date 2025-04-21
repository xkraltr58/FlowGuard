#ifndef FLOWGUARD_
#define FLOWGUARD_

#include <string>
#include <vector>

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

    private:
      int fd_;
      std::string port_name_;
      int baudrate_;
      bool is_open_;

    bool configurePort();

  };
}

#endif //FLOWGUARD_