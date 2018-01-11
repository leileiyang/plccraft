#include "FLhcInterface.h"

#include <iostream>

bool FLhcInterface::FollowTo(double height) {
  std::cout << "Follow To " << height << std::endl;
  return true;
}

bool FLhcInterface::IncrFollowTo(double height, double time) {
  std::cout << "Incr Follow To " << height << " time " << time  << std::endl;
  return true;
}


bool FLhcInterface::LiftTo(double height) {
  std::cout << "Lift To " << height << std::endl;
  return true;
}

void FLhcInterface::Update(PLC_STATUS &status) {
  status = PLC_DONE;
}
