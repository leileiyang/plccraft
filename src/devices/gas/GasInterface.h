#ifndef GASINTERFACE_H_
#define GASINTERFACE_H_

#include <map>
#include <string>

#include "../dev_cfg/PlcCfg.h"
#include "../../fl/FlBase.h"

class GasInterface {
 public:
  virtual bool Open(int gas_id);
  virtual bool Close(int gas_id);
  virtual bool SetPressure(int gas_id, double pressure);
  virtual void Update(PLC_STATUS status, int gas_id, int &on);
  virtual void Close();

  static std::map<int, std::string> gas_items;
  static std::map<int, int> gas_states;
  static std::map<int, std::string> CreateGasItems();
  static std::map<int, int> CreateGasState();

};

#endif
