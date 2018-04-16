#ifndef FOLLOWER_H_
#define FOLLOWER_H_

#include "LhcInterface.h"

class Follower {
 public:
  Follower();
  virtual ~Follower();
  virtual int FollowTo(double height);
  virtual int IncrFollowTo(double height, double time);
  virtual int LiftTo(double height);
  virtual void Update();
  virtual void Close();

  PLC_STATUS status_;

 private:
  LhcInterface *lhc_intf_;

};

#endif
