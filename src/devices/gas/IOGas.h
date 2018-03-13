#ifndef IOGAS_H_
#define IOGAS_H_

#include <map>
#include <string>
#include "GasInterface.h"
#include "../gpio/IODevice.h"

class IOGas: public GasInterface {
 public:
  explicit IOGas(IODevice *io_dev): io_dev_(io_dev) {}
  virtual bool Open(int gas_id);
  virtual bool Close(int gas_id);
  virtual bool SetPressure(int gas_id, double pressure);
  virtual void Update(PLC_STATUS status, int gas_id, int &on);
  virtual void Close();

 private:
  IODevice *io_dev_;

};

#endif
