#ifndef DEV_GAS_DEMOGAS_H_
#define DEV_GAS_DEMOGAS_H_

#include "gas_intf.h"

class DemoGas: public GasIntf {
 public:
  virtual int Open(int) {
    return GAS_OP_OK;
  }
  virtual int Close() {
    return GAS_OP_OK;
  }
  virtual int SetPressure(int, double) {
    return GAS_OP_OK;
  }
  virtual void UpdateAlarmStatus(unsigned short &) {}
};

#endif
