#ifndef IOGAS_H_
#define IOGAS_H_

#include <map>
#include <string>
#include "GasInterface.h"
#include "../gpio/IODevice.h"

class IOGas: public GasInterface {
 public:
  explicit IOGas(IODevice *io_dev): io_dev_(io_dev) {}
  virtual bool Open(GasType gas_type);
  virtual bool Close(GasType gas_type);
  virtual bool SetPressure(GasType gas_type, double pressure);
  virtual void Update(PLC_STATUS status);
  virtual void Close();

  int RegisterIODevice(IODevice *io_dev);

 private:
  IODevice *io_dev_;

};

#endif
