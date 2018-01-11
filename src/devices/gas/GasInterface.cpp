#include "GasInterface.h"

#include <iostream>

std::map<GasType, std::string> GasInterface::gas_items_ = GasInterface::CreateGasItems(); 

std::map<GasType, std::string> GasInterface::CreateGasItems() {
  std::map<GasType, std::string> gas_items; 
  gas_items[AIR] = "Air";
  gas_items[OXYGEN] = "Oxygen";
  gas_items[NITROGEN] = "Nitrogen";
  gas_items[HIGH_AIR] = "HighAir";
  gas_items[HIGH_OXYGEN] = "HighOxygen";
  gas_items[HIGH_NITROGEN] = "HighNitrogen";
  return gas_items;
}

bool GasInterface::Open(GasType gas_type) {
  std::cout << "Open " <<  gas_items_[gas_type] << std::endl;
  return true;
}

bool GasInterface::Close(GasType gas_type) {
  std::cout << "Close " << gas_items_[gas_type] << std::endl;
  return true;
}

bool GasInterface::SetPressure(GasType gas_type, double pressure) {
  std::cout << "Set " << gas_items_[gas_type] << " Pressure " << pressure \
      << std::endl;

  return true;
}


void GasInterface::Update(PLC_STATUS status) {
  status = PLC_DONE;
}
