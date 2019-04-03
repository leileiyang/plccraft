#ifndef DEV_GAS_GASINTF_H_
#define DEV_GAS_GASINTF_H_

enum GAS_OP_FEEDBACK {
  GAS_OP_NO_GAS = -1,
  GAS_OP_OK = 0,
  GAS_OPEN_DELAY,
  GAS_OPEN_DELAY_FIRST,
  GAS_OPEN_DELAY_SWITCH,
};

enum GAS_INTF_ENUM {
  GAS_INTF_DEMO,
  GAS_INTF_IO,
  GAS_INTF_MODBUS,
  GAS_INTF_OTHER,
};

class GasIntf {
 public:
  virtual int Open(int gas_id) = 0;
  virtual int Close() = 0;
  virtual int SetPressure(int gas_id, double pressure) = 0;
  virtual void UpdateAlarmStatus(unsigned short &status) = 0;
  virtual ~GasIntf() {}
};

#endif
