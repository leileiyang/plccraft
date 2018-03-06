#ifndef FOLLOWER_H_
#define FOLLOWER_H_

#include "../dev_cfg/FollowerCfg.h"
#include "FLhcInterface.h"

class PlcCraft;

class Follower {
 public:
  Follower();
  virtual ~Follower();
  virtual int FollowTo(int layer, int craft_level);
  virtual int IncrFollowTo(int layer, int craft_level);
  virtual int LiftTo(int layer);
  virtual void Update();
  virtual void Close();

  PLC_STATUS status_;

  friend class PlcCraft;

 private:
  std::vector<FollowerCfg> follower_cfg_;
  FLhcInterface *flhc_intf_;

};

#endif
