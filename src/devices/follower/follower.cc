#include "follower.h"

#include "../../nml_intf/plc_nml.h"

int Follower::FollowTo(double height) {
  if (!enable_) {
    return 0;
  }
  return follow_intf_->FollowTo(height);
}

int Follower::FollowGraduallyTo(double height, double time) {
  if (!enable_) {
    return 0;
  }
  return follow_intf_->FollowGraduallyTo(height, time);
}

int Follower::LiftTo(double height) {
  if (!enable_) {
    return 0;
  }
  return follow_intf_->LiftTo(height);
}

void Follower::MoveUp() {
  follow_intf_->MoveUp();
}

void Follower::MoveDown() {
  follow_intf_->MoveDown();
}

void Follower::MoveStop() {
  follow_intf_->MoveStop();
}

void Follower::Move(double height, bool abs) {
  follow_intf_->Move(height, abs);
}

void Follower::Home() {
  follow_intf_->Home();
}

void Follower::Calibrate() {
  follow_intf_->Calibrate();
}

void Follower::UpdateCfg() {
  switch (intf_type_) {
  case FOLLOW_INTF_IO: {
    //IoFollower *io_follower = dynamic_cast<IoFollower *>(follow_intf_);
    //IoFollowerCfg io_follower_cfg;
    //io_follower_cfg.UpdateCfg(*io_follower);
    break;
  }
  case FOLLOW_INTF_TCP:
  case FOLLOW_INTF_MODBUS:
    break;
  case FOLLOW_INTF_OTHERS:
    break;
  default:
    break;
  }
}

void Follower::UpdateStatus(PLC_LHC_STAT &lhc_stat) {
  follow_intf_->UpdateAlarmStatus(alarm_status_);
  follow_intf_->UpdateStatus(lhc_stat.status_);
  lhc_stat.alarm_status_ = alarm_status_;
}
