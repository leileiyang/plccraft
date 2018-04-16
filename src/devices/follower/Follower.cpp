#include "Follower.h"

Follower::Follower(): status_(PLC_DONE) {
  lhc_intf_ = new LhcInterface;
}

Follower::~Follower() {
  delete lhc_intf_;
}

int Follower::FollowTo(double height) {
  if (lhc_intf_->FollowTo(height)) {
    return 0; 
  } else {
    return -1;
  }
}

int Follower::IncrFollowTo(double height, double time) {
  if (lhc_intf_->IncrFollowTo(height, time)) {
    return 0; 
  } else {
    return -1;
  }
}

int Follower::LiftTo(double height) {
  if (lhc_intf_->LiftTo(height)) {
    return 0; 
  } else {
    return -1;
  }
}

void Follower::Update() {
  lhc_intf_->Update(status_);
}

void Follower::Close() {
  lhc_intf_->Close();
  status_ = PLC_DONE;
}
