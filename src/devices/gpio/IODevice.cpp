#include "IODevice.h"

#include <cassert>

int IODevice::GetPortNoByFuncId(int func_id) {
  std::map<int, int>::iterator it = io_cfg_.func_map_.find(func_id);
  if (it != io_cfg_.func_map_.end()) {
    return io_cfg_.func_map_[func_id];
  } else {
    return 0;
  }
}

int IODevice::Open(int port, int mode) {
  if (mode == PORT_NUM) {
    assert(port < IO_PORT_SIZE);
    ports_.set(port, 1);
  } else {
    ports_.set(GetPortNoByFuncId(port), 1);
  }
  return 0;
}

int IODevice::Close(int port, int mode) {
  if (mode == PORT_NUM) {
    assert(port < IO_PORT_SIZE);
    ports_.set(port, 0);
  } else {
    ports_.set(GetPortNoByFuncId(port), 0);
  }
  return 0;
}

int IODevice::GetPortState(int port, int mode) {
  if (mode == PORT_NUM) {
    assert(port < IO_PORT_SIZE);
    return ports_[port];
  } else {
    return ports_[GetPortNoByFuncId(port)];  
  }
}
