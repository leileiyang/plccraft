#ifndef DEV_FOLLOWER_FOLLOWER_H_
#define DEV_FOLLOWER_FOLLOWER_H_

#include <map>
#include <iostream>

#include "follow_intf.h"
#include "../dev.h"

class PLC_LHC_STAT;

class Follower {
 public:
  Follower(): status_(PLC_DONE), enable_(true), alarm_status_(0),
      follow_intf_(NULL), intf_type_(-1) {}

  ~Follower() {
    if (follow_intf_) {
      delete follow_intf_;
    }
  }

  void SetFollowIntf(FollowIntf *follow_intf, int intf_type) {
    follow_intf_ = follow_intf;
    intf_type_ = intf_type;
  }

  void SetEnable(bool enable) {
    enable_ = enable;
  }

  int FollowTo(double height);
  int FollowGraduallyTo(double height, double time);
  int LiftTo(double height);

  void MoveUp();
  void MoveDown();
  void Move(double height, bool abs);
  void MoveStop();
  void Home();
  void Calibrate();

  void UpdateCfg();
  void UpdateStatus(PLC_LHC_STAT &lhc_stat);

  int status_;
  bool enable_;
  unsigned int alarm_status_;
  static std::map<int, int> pos_alarmid_map;

 private:
  FollowIntf *follow_intf_;
  int intf_type_;
};

#endif
