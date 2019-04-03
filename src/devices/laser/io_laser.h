#ifndef DEV_LASER_IO_LASER_H_
#define DEV_LASER_IO_LASER_H_

#include "laser_intf.h"

class IoDevice;

class IoLaser: public LaserIntf {
 public:
  IoLaser(IoDevice *io_dev): io_dev_(io_dev) {}

  virtual int LaserOn();
  virtual int LaserOff();
  virtual int ShutterOn();
  virtual int ShutterOff();
  virtual int SetPower(double power);
  virtual int SetDutyRatio(double duty_ratio);
  virtual int SetPulseFrequency(double pulse_frequency);
  virtual int SetType(int type);

 private:
  IoDevice *io_dev_;

};

#endif
