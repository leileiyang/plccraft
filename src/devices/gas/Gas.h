#ifndef GAS_H_
#define GAS_H_

#include "../dev_cfg/GasCfg.h"
#include "GasInterface.h"

class Gas {
 public:
  Gas();
  virtual ~Gas();
  virtual int Open(int layer, int craft_level);
  virtual int Close(int layer, int craft_level);
  virtual int SetPressure(int layer, int craft_level);
  virtual void Update();

  PLC_STATUS status_;

 private:
  std::vector<GasCfg> gas_cfg_;
  GasInterface *gas_intf_;

};

#endif
