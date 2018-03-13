#ifndef IODEVICE_H_
#define IODEVICE_H_
#include <bitset>

#include "../dev_cfg/IOCfg.h"

class PlcCraft;

class IODevice {
 public:
  /// mode == 0, open the io port through por number
  /// mode == 1, open the io port through the configured function id
  int Open(int port, int mode = FUNC_ID);
  int Close(int port, int mode = FUNC_ID);
  int GetPortState(int port, int mode = FUNC_ID);

  // static resources
  std::bitset<IO_PORT_SIZE> ports_;
  IOCfg io_cfg_;

  friend class PlcCraft;

private:
  int GetPortNoByFuncId(int func_id);
};

#endif
