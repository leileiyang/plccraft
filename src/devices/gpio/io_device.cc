#include "io_device.h"

#include <cassert>

#include <rcs_prnt.hh>

int IoDevice::GetPortNoByFuncId(int func_id) {
  std::map<int, int>::iterator it = io_cfg_.func_map_.find(func_id);
  if (it != io_cfg_.func_map_.end()) {
    return io_cfg_.func_map_[func_id];
  } else {
    return 0;
  }
}

int IoDevice::Open(int port, int mode) {
  if (mode == IO_OP_PORT_NO) {
    assert(port < MAX_IO_PORT_SIZE);
    ports_.set(port, 1);
  } else {
    ports_.set(GetPortNoByFuncId(port), 1);
  }
  rcs_print("IoDevice Open Port %d\n", port);
  return 0;
}

int IoDevice::Close(int port, int mode) {
  if (mode == IO_OP_PORT_NO) {
    assert(port < MAX_IO_PORT_SIZE);
    ports_.set(port, 0);
  } else {
    ports_.set(GetPortNoByFuncId(port), 0);
  }
  rcs_print("IoDevice Close Port %d\n", port);
  return 0;
}

int IoDevice::GetPortState(int port, int mode) {
  if (mode == IO_OP_PORT_NO) {
    assert(port < MAX_IO_PORT_SIZE);
    return ports_[port];
  } else {
    return ports_[GetPortNoByFuncId(port)];  
  }
}
