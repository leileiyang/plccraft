#ifndef DEV_GAS_IOGAS_H_
#define DEV_GAS_IOGAS_H_

#include <map>

#include "gas_intf.h"

#include "../dev.h"

class IoDevice;

class IoGas: public GasIntf {
 public:
  explicit IoGas(IoDevice *io_dev): io_dev_(io_dev) {}
  virtual int Open(int gas_id);
  virtual int Close();
  virtual int SetPressure(int gas_id, double pressure);
  virtual void UpdateAlarmStatus(unsigned short &status);

  friend class IoGasCfg;

 private:
  struct GasPath {
    GasPath(): id(0) {}
    int id;
    PortCfg gas;
    PortCfg power_switch;
    PortCfg pressure_valve;
    PortCfg da;

    static PortCfg master_valve;
  };

  GasPath working_gas_;
  std::map<int, GasPath> gas_paths_;

  IoDevice *io_dev_;

  void Open();
  int SetPressure(const GasPath &gas_path, double pressure);

  void TurnOnPower();
  void TurnOffPower();
  void TurnOnPressureValve();
  void TurnOffPressureValve();
  void TurnOnMasterValve();
  void TurnOffMasterValve();

};

#endif
