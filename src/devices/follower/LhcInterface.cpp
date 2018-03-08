#include "LhcInterface.h"

#include <iostream>

bool LhcInterface::FollowTo(double height) {
  std::cout << "Follow To " << height << std::endl;
  return true;
}

bool LhcInterface::IncrFollowTo(double height, double time) {
  std::cout << "Incr Follow To " << height << " time " << time  << std::endl;
  return true;
}


bool LhcInterface::LiftTo(double height) {
  std::cout << "Lift To " << height << std::endl;
  return true;
}

void LhcInterface::Update(PLC_STATUS &status) {
  status = PLC_DONE;
}

void LhcInterface::Close() {
  std::cout << "Close the follower device." << std::endl;
}
