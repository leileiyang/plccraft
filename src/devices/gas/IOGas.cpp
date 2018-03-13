#include "IOGas.h"

#include <iostream>

bool IOGas::Open(int gas_id) {
  std::cout << "open, gas id:" << gas_id << std::endl;
  io_dev_->Open(gas_id);
  return true;
}

bool IOGas::Close(int gas_id) {
  std::cout << "Close, gas id:" << gas_id << std::endl;
  io_dev_->Close(gas_id);
  return true;
}

bool IOGas::SetPressure(int gas_id, double pressure) {
  std::cout << "set gas:" << gas_id << " pressuer:" << pressure << std::endl;
  return true;
}


void IOGas::Update(PLC_STATUS status, int gas_id, int &on) {
  status = PLC_DONE;
  // if the gas id does not map to any io port,
  // you will get the port 0 state
  on = io_dev_->GetPortState(gas_id);
}

void IOGas::Close() {
  std::cout << "Close the gas device." << std::endl;
}
