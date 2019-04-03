#ifndef DEV_LASER_DEMO_LASER_H_
#define DEV_LASER_DEMO_LASER_H_

#include "laser_intf.h"

class DemoLaser: public LaserIntf {
 public:
  virtual int LaserOn() {
    return 0;
  }
  virtual int LaserOff() {
    return 0;
  }
  virtual int ShutterOn() {
    return 0;
  }
  virtual int ShutterOff() {
    return 0;
  }
  virtual int SetPower(double) {
    return 0;
  }
  virtual int SetDutyRatio(double) {
    return 0;
  }
  virtual int SetPulseFrequency(double) {
    return 0;
  }
  virtual int SetType(int type) {
    return 0;
  }

};

#endif
