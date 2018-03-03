#ifndef GASCFG_H_
#define GASCFG_H_

#include "../../fl/FlBaseCfg.h"
#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <iostream>

class GasCfg {
 public:
  GasCfg(): gas_(CRAFT_LEVELS, VACUO), pressure_(CRAFT_LEVELS, 0.){}

  std::vector<GasType> gas_;
  std::vector<double> pressure_;
  void Show() {
    for (int i = 0; i < CRAFT_LEVELS; i++) {
      std::cout << gas_[i] << " pressure:" << pressure_[i] << std::endl;
    }
  }

};

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive &ar, GasCfg &cfg, const unsigned int version) {
  ar & cfg. gas_;
  ar & cfg.pressure_;
}

}
}

#endif
