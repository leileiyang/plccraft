#ifndef GAS_H_
#define GAS_H_

#include "../dev_cfg/GasCfg.h"
#include "GasInterface.h"

class PlcCraft;

class Gas {
 public:
  Gas();
  ~Gas();
  int Open(GasType gas);
  int Open(int layer, int craft_level);
  int Close(int layer, int craft_level);
  int Close(GasType gas);
  int SetPressure(int layer, int craft_level);
  int SetPressure(GasType gas);
  void Update();

  virtual void Close();
  PLC_STATUS status_;

  friend class PlcCraft;

 private:
  // static configure
  std::vector<GasCfg> gas_cfg_;
  GasInterface *gas_intf_;

  // state information
  int working_gas_;

};

#endif
