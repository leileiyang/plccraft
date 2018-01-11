#include "Gas.h"

Gas::Gas(): status_(PLC_DONE), gas_cfg_(CRAFT_LAYERS, GasCfg()) {
  gas_intf_ = new GasInterface;
}

Gas::~Gas() {
  delete gas_intf_;
}

int Gas::Open(int layer, int craft_level) {
  if (layer >= CRAFT_LAYERS || craft_level >= CRAFT_LEVELS) {
    return -1;
  }
  if (gas_intf_->Open(gas_cfg_[layer].gas_[craft_level])) {
    return 0; 
  } else {
    return -2;
  }
}

int Gas::Close(int layer, int craft_level) {
  if (layer >= CRAFT_LAYERS || craft_level >= CRAFT_LEVELS) {
    return -1;
  }
  if (gas_intf_->Close(gas_cfg_[layer].gas_[craft_level])) {
    return 0; 
  } else {
    return -2;
  }
}

int Gas::SetPressure(int layer, int craft_level) {
  if (layer >= CRAFT_LAYERS || craft_level >= CRAFT_LEVELS) {
    return -1;
  }
  if (gas_intf_->SetPressure(gas_cfg_[layer].gas_[craft_level], \
      gas_cfg_[layer].pressure_[craft_level])) {

    return 0; 
  } else {
    return -2;
  }
}

void Gas::Update() {
  gas_intf_->Update(status_);
}
