#ifndef GASCFG_H_
#define GASCFG_H_

#include "../../fl/FlBaseCfg.h"
#include <vector>

class GasCfg {
 public:
  GasCfg(): gas_(CRAFT_LEVELS, VACUO), pressure_(CRAFT_LEVELS, 0.){}

  std::vector<GasType> gas_;
  std::vector<double> pressure_;

};

#endif
