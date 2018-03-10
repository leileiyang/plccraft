#ifndef FLHCINTERFACE_H_
#define FLHCINTERFACE_H_

#include <string>
#include "../../fl/FlBase.h"

class LhcInterface {
 public:
  virtual bool FollowTo(double height);
  virtual bool IncrFollowTo(double height, double time);
  virtual bool LiftTo(double height);
  virtual void Update(PLC_STATUS &status);
  virtual void Close();

};

#endif
