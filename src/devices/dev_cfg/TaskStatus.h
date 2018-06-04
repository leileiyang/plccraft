#ifndef DEV_CFG_TASKSTATUS_H_
#define DEV_CFG_TASKSTATUS_H_

#include <vector>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class TaskStatus {
 public:
  TaskStatus() {}

  int mk_status_;
  int mk_task_mode_;
  int mk_echo_serial_number_;

  double x_;
  double y_;
  double z_;
  double a_;
  double b_;
  double c_;

};

namespace boost {
namespace serialization {

template <class Archive>
void serialize(Archive &ar, TaskStatus &cfg, const unsigned int version) {
  ar & cfg.mk_status_;
  ar & cfg.mk_task_mode_;
  ar & cfg.mk_echo_serial_number_;
  ar & cfg.x_;
  ar & cfg.y_;
  ar & cfg.z_;
  ar & cfg.a_;
  ar & cfg.b_;
  ar & cfg.c_;
}

}
}

#endif
