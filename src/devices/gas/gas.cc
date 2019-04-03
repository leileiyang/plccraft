#include "gas.h"

#include "io_gas.h"
#include "../../nml_intf/plc_nml.h"

typedef std::pair<int, int> pair_t;

static const pair_t gas_array[] = {
  pair_t(GAS_AIR, 0),
  pair_t(GAS_O2, 1),
  pair_t(GAS_N2, 2),
  pair_t(GAS_HIGH_AIR, 3),
  pair_t(GAS_HIGH_O2, 4),
  pair_t(GAS_HIGH_N2, 5),
};

std::map<int, int> Gas::gas_id_pos_map(gas_array,
    gas_array + sizeof(gas_array) / sizeof(gas_array[0]));

int Gas::Open(int gas_id) {
  if (!enable_) {
    return GAS_OP_OK;
  }
  int ret = gas_intf_->Open(gas_id);
  if (ret) {
    ;
  } else {
    if (current_gas_ < 0) {
      ret = GAS_OPEN_DELAY_FIRST;
    } else if (current_gas_ != gas_id) {
      ret = GAS_OPEN_DELAY_SWITCH;
    } else {
      ret = GAS_OPEN_DELAY;
    }
    current_gas_ = gas_id;
    SetGasStatus(gas_id, 1);
  }
  return ret; 
}

int Gas::Close() {
  if (!enable_) {
    return 0;
  }
  gas_intf_->Close();
  SetGasStatus(current_gas_, 0);
  return 0;
}

int Gas::SetPressure(int gas_id, double pressure) {
  if (!enable_) {
    return 0;
  }
  int ret = gas_intf_->SetPressure(gas_id, pressure);
  if (ret) {
    ;
  } else {
    current_pressure_ = pressure;
    current_gas_ = gas_id;
  }
  return ret;
}

void Gas::UpdateCfg() {
  switch (intf_type_) {
    case GAS_INTF_IO:
      break;
    case GAS_INTF_MODBUS:
      break;
    case GAS_INTF_DEMO:
    default:
      break;
  }
}

void Gas::UpdateStatus(PLC_GAS_STAT &gas_stat) {
  gas_stat.current_gas_ = current_gas_;
  gas_stat.current_pressure_ = current_pressure_;
  gas_stat.gas_status_ = gas_status_;
  gas_stat.status_ = status_;
  gas_stat.alarm_status_ = alarm_status_;
}

void Gas::SetGasStatus(int gas_id, int on) {
  if (on) {
    gas_status_ |= 1UL << gas_id_pos_map[gas_id];
  } else {
    gas_status_ &= ~(1UL << gas_id_pos_map[gas_id]);
  }
}
