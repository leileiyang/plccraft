#ifndef DEV_LASER_LASER_H
#define DEV_LASER_LASER_H

#include "../../nml_intf/plc_nml.h"

class LaserIntf;

class Laser {
 public:
  int LaserOn();
  int LaserOff();
  int ShutterOn();
  int ShutterOff();
  int SetPower(double power);
  int SetDutyRatio(double duty_ratio);
  int SetPulseFrequency(double pulse_frequency);
  int SetType(int type);
  void SetIntf(LaserIntf* laser_intf, int intf_type) {
    laser_intf_ = laser_intf;
    intf_type_ = intf_type;
  }

  void UpdateStatus(PLC_LASER_STAT &laser_stat) {
    laser_stat = laser_stat_;
  }

 private:
  LaserIntf *laser_intf_;
  int intf_type_;

  PLC_LASER_STAT laser_stat_;

};

#endif
