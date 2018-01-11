#ifndef FOLLOWER_H_
#define FOLLOWER_H_

#include "FollowerCfg.h"
#include "FLhcInterface.h"

class Follower {
 public:
  Follower();
  virtual ~Follower();
  virtual int FollowTo(int layer, int craft_level);
  virtual int IncrFollowTo(int layer, int craft_level);
  virtual int LiftTo(int layer);
  virtual void Update();

  PLC_STATUS status_;

 private:
  std::vector<FollowerCfg> follower_cfg_;
  FLhcInterface *flhc_intf_;

};

#endif
