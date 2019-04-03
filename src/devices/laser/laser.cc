#include "laser.h"

#include <rcs_prnt.hh>

#include "laser_intf.h"

int Laser::LaserOn() {
  rcs_print("LaserOn\n");
  laser_stat_.on_ = 1;
  return laser_intf_->LaserOn();
}

int Laser::LaserOff() {
  rcs_print("LaserOff\n");
  laser_stat_.on_ = 0;
  return laser_intf_->LaserOff();
}

int Laser::ShutterOn() {
  rcs_print("ShutterOn\n");
  laser_stat_.shutter_ = 1;
  return laser_intf_->ShutterOn();
}

int Laser::ShutterOff() {
  rcs_print("ShutterOff\n");
  laser_stat_.shutter_ = 0;
  return laser_intf_->ShutterOff();
}

int Laser::SetPower(double power) {
  rcs_print("Laser Set Power %f\n", power);
  laser_stat_.power_ = power;
  return laser_intf_->SetPower(power);
}

int Laser::SetDutyRatio(double duty_ratio) {
  rcs_print("Laser Set DutyRatio %f\n", duty_ratio);
  laser_stat_.pwm_ = duty_ratio;
  return laser_intf_->SetDutyRatio(duty_ratio);
}

int Laser::SetPulseFrequency(double pulse_frequency) {
  rcs_print("Laser Set Pulse Frequency %f\n", pulse_frequency);
  laser_stat_.frequency_ = pulse_frequency;
  return laser_intf_->SetPulseFrequency(pulse_frequency);
}

int Laser::SetType(int type) {
  rcs_print("Laser Set Type %d\n", type);
  laser_stat_.type_ = type;
  return laser_intf_->SetType(type);
}
