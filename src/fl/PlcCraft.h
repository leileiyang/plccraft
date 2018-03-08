#ifndef PLCCRAFT_H_
#define PLCCRAFT_H_

#include <queue>

#include "../devices/gas/Gas.h"
#include "../devices/follower/Follower.h"
#include "../devices/dev_cfg/DeviceCfg.h"
#include "../devices/dev_cfg/PlcCfg.h"


enum PLC_TASK_EXEC_ENUM {
  PLC_TASK_EXEC_ERROR = 1,
  PLC_TASK_EXEC_DONE = 2,
  PLC_TASK_EXEC_WAITING_FOR_GAS = 3,
  PLC_TASK_EXEC_WAITING_FOR_LHC = 4,
  PLC_TASK_EXEC_WAITING_FOR_LASER = 5,
  PLC_TASK_EXEC_WAITING_FOR_DELAY = 6,
};

class PlcCraft {
 public:
  PlcCraft();
  bool Initialize();
  void AddCmd(PLC_CMD_ENUM command);
  PLC_STATUS IssueCmd();
  void Update();
  void UpdateDeviceCfg();
  void LoadCraft(int craft_layer);
  PLC_STATUS Execute();

  int craft_layer_;
  PLC_STATUS status_;
  PLC_TASK_EXEC_ENUM exec_state_; 

 private:
  int execute_error_;
  std::queue<PLC_CMD_ENUM> cmds_;
  PLC_CMD_ENUM cmd_;
  const PLC_CMD_ENUM GetNextCmd();
  const std::size_t GetCmdQueueSize();
  void DetachLastCmd();
  int DoCmd();
  PLC_TASK_EXEC_ENUM CheckPostCondition();
  void TaskAbort();

  DeviceCfg device_cfg_;
  Gas *gas_;
  Follower *follower_;

};

#endif
