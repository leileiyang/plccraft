#include "Gas.h"
#include "IOGas.h"

Gas::Gas(): status_(PLC_DONE),
    gas_intf_(NULL), working_gas_(PLC_CMD_NONE), on_(0) {}

Gas::~Gas() {}

int Gas::ConnectInterface(GasInterface *gas_intf) {
  if (gas_intf) {
    gas_intf_ = gas_intf;
    return 0;
  } else {
    return -1;
  }
}

int Gas::Open(int gas_id) {
  if (gas_intf_->Open(gas_id)) {
    working_gas_ = gas_id;
    return 0; 
  } else {
    return -1;
  }
}

int Gas::Close(int gas_id) {
  if (gas_intf_->Close(gas_id)) {
    working_gas_ = PLC_CMD_NONE;
    return 0; 
  } else {
    return -1;
  }
}

int Gas::SetPressure(int gas_id, double pressure) {
  if (gas_intf_->SetPressure(gas_id, pressure)) {
    return 0; 
  } else {
    return -1;
  }
}

void Gas::Update() {
  gas_intf_->Update(status_, working_gas_, on_);
}

void Gas::Close() {
  gas_intf_->Close();
  status_ = PLC_DONE;
}
