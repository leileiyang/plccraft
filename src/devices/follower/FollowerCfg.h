#ifndef FOLLOWERCFG_H_
#define FOLLOWERCFG_H_

#include "../../fl/FlBaseCfg.h"
#include <vector>

class FollowerCfg {
 public:
  FollowerCfg(): height_(CRAFT_LEVELS, 0.), incr_enable_(CRAFT_LEVELS, false),
      incr_time_(CRAFT_LEVELS, 0.), lift_height_(0.) {}

  std::vector<double> height_;
  std::vector<bool> incr_enable_;
  std::vector<double> incr_time_;
  double lift_height_;

};

#endif
