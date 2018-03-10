#ifndef GASINTERFACE_H_
#define GASINTERFACE_H_

#include <map>
#include <string>

#include "../dev_cfg/PlcCfg.h"
#include "../../fl/FlBase.h"

class GasInterface {
 public:
  virtual bool Open(GasType gas_type);
  virtual bool Close(GasType gas_type);
  virtual bool SetPressure(GasType gas_type, double pressure);
  virtual void Update(PLC_STATUS status);
  virtual void Close();

  static std::map<GasType, std::string> gas_items_;
  static std::map<GasType, std::string> CreateGasItems();
};

#endif
