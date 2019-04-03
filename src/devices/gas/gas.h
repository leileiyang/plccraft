#ifndef DEV_GAS_GAS_H_
#define DEV_GAS_GAS_H_

#include <map>
#include <iostream>

#include "gas_intf.h"
#include "../dev.h"

enum GAS_ID_ENUM {
  GAS_AIR,
  GAS_O2,
  GAS_N2,
  GAS_HIGH_AIR,
  GAS_HIGH_O2,
  GAS_HIGH_N2,
};

class PLC_GAS_STAT;

class Gas {
 public:
  Gas(): status_(PLC_DONE), enable_(true), gas_intf_(NULL), intf_type_(-1),
      current_gas_(-1), current_pressure_(0.),
      gas_status_(0), alarm_status_(0) {}

  ~Gas() {
    if (gas_intf_) {
      delete gas_intf_;
    }
  }

  int Open(int gas_id);
  int Close();
  int SetPressure(int gas_id, double pressure);

  void UpdateCfg();
  void UpdateStatus(PLC_GAS_STAT &gas_stat);

  void SetEnable(bool enable) {
    enable_ = enable;
  }
  void SetIntf(GasIntf *gas_intf, int intf_type) {
    gas_intf_ = gas_intf;
    intf_type_ = intf_type;
  }

  int status_;
  bool enable_;

 private:
  GasIntf* gas_intf_;
  int intf_type_;

  int current_gas_;
  double current_pressure_;
  unsigned short gas_status_;
  unsigned short alarm_status_;

  void SetGasStatus(int gas_id, int on);

  static std::map<int, int> gas_id_pos_map;
};

#endif
