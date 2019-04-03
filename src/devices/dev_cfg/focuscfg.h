#ifndef DEVICES_DEVCFG_FOCUSCFG_H_
#define DEVICES_DEVCFG_FOCUSCFG_H_

#include "PlcCfg.h"
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>

class FocusCfg {
 public:
  FocusCfg(): position_(CRAFT_LEVELS, 0) {}

  std::vector<double> position_;
  void Show() {
    for (int i = 0; i < CRAFT_LEVELS; i++) {
      std::cout << "position:" << position_[i] << std::endl;
    }
  }
};

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive &ar, FocusCfg &cfg, const unsigned int version) {
  ar & cfg.position_;
}

}
}

#endif // DEVICES_DEVCFG_FOCUSCFG_H_
