#include "GasInterface.h"

#include <iostream>

std::map<int, std::string> GasInterface::gas_items = \
    GasInterface::CreateGasItems(); 

std::map<int, int> GasInterface::gas_states = \
    GasInterface::CreateGasState();

std::map<int, int> GasInterface::CreateGasState() {
  std::map<int, int> states;
  states[GAS_AIR] = 0;
  states[GAS_O2] = 0;
  states[GAS_N2] = 0;
  states[GAS_HIGH_AIR] = 0;
  states[GAS_HIGH_O2] = 0;
  states[GAS_HIGH_N2] = 0;
  return states;
}

std::map<int, std::string> GasInterface::CreateGasItems() {
  std::map<int, std::string> items;
  items[GAS_AIR] = "Air";
  items[GAS_O2] = "Oxygen";
  items[GAS_N2] = "Nitrogen";
  items[GAS_HIGH_AIR] = "HighAir";
  items[GAS_HIGH_O2] = "HighOxygen";
  items[GAS_HIGH_N2] = "HighNitrogen";
  return items;
}

bool GasInterface::Open(int gas_id) {
  std::cout << "Open " << gas_items[gas_id] << std::endl;
  gas_states[gas_id] = 1;
  return true;
}

bool GasInterface::Close(int gas_id) {
  std::cout << "Close " << gas_items[gas_id] << std::endl;
  gas_states[gas_id] = 0;
  return true;
}

bool GasInterface::SetPressure(int gas_id, double pressure) {
  std::cout << "Set " << gas_items[gas_id] << " Pressure " << pressure \
      << std::endl;

  return true;
}


void GasInterface::Update(PLC_STATUS status, int gas_id, int &on) {
  status = PLC_DONE;
  on = gas_states[gas_id];
}

void GasInterface::Close() {
  std::cout << "Close the gas device." << std::endl;
}
