#ifndef GAS_H_
#define GAS_H_

#include "GasInterface.h"

class Gas {
 public:
  Gas();
  ~Gas();
  int Open(int gas_id);
  int Close(int gas_id);
  int SetPressure(int gas_id, double pressure);

  // Before using the gas device, you must connect a
  // gas interface class instances first.
  int ConnectInterface(GasInterface *gas_intf);
  void Update();

  virtual void Close();
  PLC_STATUS status_;

 private:
  // static configure
  GasInterface *gas_intf_;

  // state information
  int working_gas_;
  // 0->off; 1->on
  int on_;

};

#endif
