#include "Gas.h"

#include "GasInterface.h"

Gas::Gas(): gas_cfg_(LAYER_NUM, GasCfg()), gas_intf_(NULL) {
  gas_intf_ = new GasInterface;
}

int Gas::Open(int layer, int gas_kind) {
  if (layer >= LAYER_NUM || gas_kind >= GAS_KIND_NUM) {
    return -1;
  }
  if (gas_intf_->Open(gas_cfg_[layer].gas_[gas_kind])) {
    return 0; 
  } else {
    return -2;
  }
}

int Gas::Close(int layer, int gas_kind) {
  if (layer >= LAYER_NUM || gas_kind >= GAS_KIND_NUM) {
    return -1;
  }
  if (gas_intf_->Close(gas_cfg_[layer].gas_[gas_kind])) {
    return 0; 
  } else {
    return -2;
  }
}

int Gas::SetPressure(int layer, int gas_kind) {
  if (layer >= LAYER_NUM || gas_kind >= GAS_KIND_NUM) {
    return -1;
  }
  if (gas_intf_->SetPressure(gas_cfg_[layer].gas_[gas_kind], \
      gas_cfg_[layer].pressure_[gas_kind])) {

    return 0; 
  } else {
    return -2;
  }
}
