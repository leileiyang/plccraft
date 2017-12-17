#ifndef GASCFG_H_
#define GASCFG_H_

#include "GasBase.h"
#include <vector>

#define GAS_KIND_NUM 4

class GasCfg {
 public:
  GasCfg(): gas_(GAS_KIND_NUM, VACUO), pressure_(GAS_KIND_NUM, 0.){}

  std::vector<GasType> gas_;
  std::vector<double> pressure_;

};

#endif
