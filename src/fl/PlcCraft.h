#ifndef PLCCRAFT_H_
#define PLCCRAFT_H_

#include <queue>

#include "../devices/gas/Gas.h"
#include "../devices/follower/Follower.h"
#include "../devices/gpio/IODevice.h"
#include "../devices/dev_cfg/DeviceCfg.h"

enum PLC_EXEC_ENUM {
  PLC_EXEC_ERROR = 1,
  PLC_EXEC_DONE = 2,
  PLC_EXEC_WAITING_FOR_GAS = 3,
  PLC_EXEC_WAITING_FOR_LHC = 4,
  PLC_EXEC_WAITING_FOR_LASER = 5,
  PLC_EXEC_WAITING_FOR_DELAY = 6,
};

class PlcCraft {
 public:
  PlcCraft();
  ~PlcCraft();
  bool Initialize();
  void AddCmd(PlcCmd command);
  PLC_STATUS IssueCmd();
  void Update();
  void UpdateDeviceCfg();
  void LoadCraft(int craft_layer);
  PLC_STATUS Execute();

  int craft_layer_;
  PLC_STATUS status_;
  PLC_EXEC_ENUM exec_state_; 

 private:
  int execute_error_;
  std::queue<PlcCmd> cmds_;
  PlcCmd cmd_;
  const PlcCmd GetNextCmd();
  const std::size_t GetCmdQueueSize();
  void DetachLastCmd();
  int DoCmd();
  PLC_EXEC_ENUM CheckPostCondition();
  void TaskAbort();

  DeviceCfg device_cfg_;
  PlcCfg plc_cfg_;
  Gas *gas_;
  Follower *follower_;
  IODevice *output_;

};

#endif
