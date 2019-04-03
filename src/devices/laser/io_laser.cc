#include "io_laser.h"

#include "../gpio/io_device.h"

int IoLaser::LaserOn() {
  //return io_dev_->Open(LASER_SWITCH);
  return 0;
}

int IoLaser::LaserOff() {
  //return io_dev_->Close(LASER_SWITCH);
  return 0;
}

int IoLaser::ShutterOn() {
  //return io_dev_->Open(LASER_SHUTTER);
  return 0;
}
int IoLaser::ShutterOff() {
  //return io_dev_->Close(LASER_SHUTTER);
  return 0;
}

int IoLaser::SetPower(double power) {
  return 0;
}

int IoLaser::SetDutyRatio(double duty_ratio) {
  return 0;
}

int IoLaser::SetPulseFrequency(double pulse_frequency) {
  return 0;
}

int IoLaser::SetType(int type) {
  return 0;
}
