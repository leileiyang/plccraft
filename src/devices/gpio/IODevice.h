#ifndef IODEVICE_H_
#define IODEVICE_H_
#include <map>
#include <bitset>

class IODevice {
 public:
  int Open(int port);
  int Close(int port);

  // static resources
  std::bitset<17> ports_;
};

#endif
