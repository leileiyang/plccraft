#ifndef DEV_GAS_GASINTF_H_
#define DEV_GAS_GASINTF_H_

enum FOLLOW_OP_FEEDBACK {
  FOLLOW_OP_OK,
  FOLLOW_OP_ERROR,
};

enum FOLLOW_INTF_ENUM {
  FOLLOW_INTF_IO,
  FOLLOW_INTF_TCP,
  FOLLOW_INTF_MODBUS,
  FOLLOW_INTF_OTHERS,
};

class FollowIntf {
 public:
  virtual int FollowTo(double height) = 0;
  virtual int FollowGraduallyTo(double height, double time) = 0;
  virtual int LiftTo(double height) = 0;

  virtual void MoveUp() = 0;
  virtual void MoveDown() = 0;
  virtual void Move(double height, bool abs) = 0;
  virtual void MoveStop() = 0;

  virtual void Home() = 0;
  virtual void Calibrate() = 0;

  virtual void UpdateAlarmStatus(unsigned int &status) = 0;
  virtual void UpdateStatus(int &status) = 0;
  virtual ~FollowIntf() {}

};

#endif
