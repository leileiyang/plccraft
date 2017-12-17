#ifndef GAS_H_
#define GAS_H_

#include "GasCfg.h"

#include <vector>

#define LAYER_NUM 18

class GasInterface;

class Gas {
 public:
  Gas();
  virtual int Open(int layer, int gas_kind);
  virtual int Close(int layer, int gas_kind);
  virtual int SetPressure(int layer, int gas_kind);

 private:
  std::vector<GasCfg> gas_cfg_;
  GasInterface *gas_intf_;

};

#endif
