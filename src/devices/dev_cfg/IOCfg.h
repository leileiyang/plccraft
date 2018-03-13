#ifndef IOCFG_H_
#define IOCFG_H_

#include "PlcCfg.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <iostream>
#include <map>

class IOCfg {
 public:
  IOCfg(): group_id_(0) {}

  int group_id_;

  // function id => port number map
  std::map<int, int> func_map_;

  void Show() {
    std::cout << "group id:" << group_id_ << std::endl;
    for (std::map<int, int>::iterator it = func_map_.begin();
        it != func_map_.end(); it++) {

      std::cout << it->first << "=>" << it->second << std::endl;
    }
  }
};

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive &ar, IOCfg &cfg, const unsigned int version) {
  ar & cfg.group_id_;
  ar & cfg.func_map_;
}

}
}

#endif
