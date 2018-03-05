#ifndef FOLLOWERCFG_H_
#define FOLLOWERCFG_H_

#include "../../fl/FlBaseCfg.h"
#include <vector>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class FollowerCfg {
 public:
  FollowerCfg(): height_(CRAFT_LEVELS, 0.), incr_enable_(CRAFT_LEVELS, false),
      incr_time_(CRAFT_LEVELS, 0.), lift_height_(0.) {}

  std::vector<double> height_;
  std::vector<bool> incr_enable_;
  std::vector<double> incr_time_;
  double lift_height_;

  void Show() {
    for (int i = 0; i < CRAFT_LEVELS; i++) {
      std::cout << i << "->height:" << height_[i] << " incr_enable:" <<
        incr_enable_[i] << " incr_time:" << incr_time_[i] << std::endl;
    }
    std::cout << "lift height:" << lift_height_ << std::endl;
  }
};

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive &ar, FollowerCfg &cfg, const unsigned int version) {
  ar & cfg.height_;
  ar & cfg.incr_enable_;
  ar & cfg.incr_time_;
  ar & cfg.lift_height_;
}

}
}

#endif
