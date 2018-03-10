#include "Follower.h"

Follower::Follower(): status_(PLC_DONE),
    follower_cfg_(CRAFT_LAYERS, FollowerCfg()) {

  lhc_intf_ = new LhcInterface;
}

Follower::~Follower() {
  delete lhc_intf_;
}

int Follower::FollowTo(int layer, int craft_level) {
  if (layer >= CRAFT_LAYERS || craft_level >= CRAFT_LEVELS) {
    return -1;
  }
  if (follower_cfg_[layer].no_follow_) {
    return 0;
  }
  if (lhc_intf_->FollowTo(follower_cfg_[layer].height_[craft_level])) {
    return 0; 
  } else {
    return -2;
  }
}

int Follower::IncrFollowTo(int layer, int craft_level) {
  if (layer >= CRAFT_LAYERS || craft_level >= CRAFT_LEVELS) {
    return -1;
  }

  if (follower_cfg_[layer].no_follow_) {
    return 0;
  }

  if (craft_level == CRAFT_CUTTING) {
      return 0;
  }
  if (!follower_cfg_[layer].incr_enable_[craft_level]) {
      return 0;
  }
  if (lhc_intf_->IncrFollowTo(\
      follower_cfg_[layer].height_[craft_level-1],\
      follower_cfg_[layer].incr_time_[craft_level])) {

    return 0; 
  } else {
    return -2;
  }
}

int Follower::LiftTo(int layer) {
  if (layer >= CRAFT_LAYERS) {
    return -1;
  }
  if (lhc_intf_->LiftTo(follower_cfg_[layer].lift_height_)) {
    return 0; 
  } else {
    return -2;
  }
}

void Follower::Update() {
  lhc_intf_->Update(status_);
}

void Follower::Close() {
  lhc_intf_->Close();
  status_ = PLC_DONE;
}
