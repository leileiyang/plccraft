#ifndef DEV_LASER_INTF_H_
#define DEV_LASER_INTF_H_

enum LASER_INTF_ENUM {
  LASER_INTF_DEMO,
  LASER_INTF_IO,
  LASER_INTF_MODBUS,
  LASER_INTF_OTHER,
};

class LaserIntf {
 public:
  virtual int LaserOn() = 0;
  virtual int LaserOff() = 0;
  virtual int ShutterOn() = 0;
  virtual int ShutterOff() = 0;
  virtual int SetPower(double power) = 0;
  virtual int SetDutyRatio(double duty_ratio) = 0;
  virtual int SetPulseFrequency(double pulse_frequency) = 0;
  virtual int SetType(int type) = 0;
  
};

#endif
