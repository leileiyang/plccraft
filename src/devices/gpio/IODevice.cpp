#include "IODevice.h"

int IODevice::Open(int port) {
  ports_.set(port, 1);
  return 0;
}

int IODevice::Close(int port) {
  ports_.set(port, 0);
  return 0;
}
