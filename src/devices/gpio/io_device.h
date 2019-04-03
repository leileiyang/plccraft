#ifndef DEV_GPIO_IODEVICE_H_
#define DEV_GPIO_IODEVICE_H_

#include <bitset>
#include <map>

#define MAX_IO_PORT_SIZE 16

enum IO_OP_MODE {
  IO_OP_PORT_NO,
  IO_OP_FUNC_ID,
};

struct IoCfg {
  int dev_id;
  // function id -> port number
  std::map<int, int> func_map_;
};

class IoDevice {
 public:
  /// mode == 0, open the io port through por number
  /// mode == 1, open the io port through the configured function id
  int Open(int port, int mode = IO_OP_FUNC_ID);
  int Close(int port, int mode = IO_OP_FUNC_ID);
  int GetPortState(int port, int mode = IO_OP_FUNC_ID);

  // static resources
  std::bitset<MAX_IO_PORT_SIZE> ports_;
  IoCfg io_cfg_;

 private:
  int dev_id_;
  int GetPortNoByFuncId(int func_id);
};

#endif
