#ifndef GAS_H_
#define GAS_H_

#include "../dev_cfg/GasCfg.h"
#include "GasInterface.h"

class PlcCraft;

class Gas {
 public:
  Gas();
  virtual ~Gas();
  virtual int Open(int layer, int craft_level);
  virtual int Close(int layer, int craft_level);
  virtual int SetPressure(int layer, int craft_level);
  virtual void Update();

  virtual void Close();
  PLC_STATUS status_;

  friend class PlcCraft;

 private:
  std::vector<GasCfg> gas_cfg_;
  GasInterface *gas_intf_;

};

#endif
