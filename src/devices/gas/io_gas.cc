#include "io_gas.h"

#include "../../dev/gpio/io_device.h"

int IoGas::Open(int gas_id) {
  std::map<int, GasPath>::iterator it = gas_paths_.find(gas_id);
  if (it == gas_paths_.end()) {
    return GAS_OP_NO_GAS;
  } else {
    if (it->second.id == working_gas_.id) {
      Open();
    } else {
      Close();
      TurnOffPower();
      if (working_gas_.pressure_valve.num != it->second.pressure_valve.num) {
        TurnOffPressureValve();
      }
      working_gas_ = it->second;
      Open();
    }
    return GAS_OP_OK;
  }
}

void IoGas::Open() {
  TurnOnMasterValve();
  TurnOnPressureValve();
  TurnOnPower();
  // to do: open gas
}

int IoGas::Close() {
  TurnOffPressureValve();
  TurnOffMasterValve();
  // to do: close gas
  return GAS_OP_OK;
}

int IoGas::SetPressure(int gas_id, double pressure) {
  std::map<int, GasPath>::iterator it = gas_paths_.find(gas_id);
  if (it == gas_paths_.end()) {
    return GAS_OP_NO_GAS;
  } else {
    // to do: set gas pressure through io device
    return GAS_OP_OK;
  }
}

void IoGas::UpdateAlarmStatus(unsigned short &status) {
}

void IoGas::TurnOnPower() {
  if (working_gas_.power_switch.num) {
    ;
  }
}

void IoGas::TurnOffPower() {
  if (working_gas_.power_switch.num) {
    ;
  }
}

void IoGas::TurnOnPressureValve() {
  if (working_gas_.pressure_valve.num) {
    ;
  }
}

void IoGas::TurnOffPressureValve() {
  if (working_gas_.pressure_valve.num) {
    ;
  }
}

void IoGas::TurnOnMasterValve() {
  if (working_gas_.master_valve.num) {
    ;
  }
}

void IoGas::TurnOffMasterValve() {
  if (working_gas_.master_valve.num) {
    ;
  }
}

