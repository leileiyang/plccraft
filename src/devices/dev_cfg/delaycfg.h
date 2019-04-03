#ifndef DELAY_CFG_H_
#define DELAY_CFG_H_

#include "PlcCfg.h"
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>

class DelayCfg {
 public:
  DelayCfg(): stay_(CRAFT_LEVELS, 3000),
      laser_off_blow_time_(CRAFT_LEVELS, 500),
      blow_enable_(CRAFT_LEVELS, false) {}

  std::vector<double> stay_;
  std::vector<double> laser_off_blow_time_;
  std::vector<bool> blow_enable_;
  
  void Show() {
    for (int i = 0; i < CRAFT_LEVELS; i++) {
      std::cout << "stay time:" << stay_[i]
          << " laser off blow:" << laser_off_blow_time_[i]
          << " blow enable:" << blow_enable_[i] << std::endl;

    }
  }
};

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive &ar, DelayCfg &cfg, const unsigned int version) {
  ar & cfg.stay_;
  ar & cfg.laser_off_blow_time_;
  ar & cfg.blow_enable_;
}

}
}

#endif
