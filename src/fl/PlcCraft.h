#ifndef PLCCRAFT_H_
#define PLCCRAFT_H_

#include <queue>
#include <vector>

#include "../devices/gas/Gas.h"
#include "../devices/follower/Follower.h"
#include "../devices/gpio/IODevice.h"
#include "../devices/dev_cfg/CfgSubscriber.h"

#include "JobSeeker.h"
#include "JobLoader.h"

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
  void AddCmd(const PlcCmd &command);
  PLC_STATUS IssueCmd();
  void Update();
  void UpdateDeviceCfg();
  void LoadCraftProcesses(int motion_line);
  void TaskAbort();
  PLC_STATUS Execute();

  int PullCommand(PlcCmd &cmd);

  PLC_STATUS status_;
  PLC_EXEC_ENUM exec_state_; 

  bool OpenJobImage(const char *file_name);
  bool ReOpenJobImage();
  void CloseJobImage();

 private:
  int execute_error_;
  std::queue<PlcCmd> cmds_;
  PlcCmd cmd_;
  const PlcCmd GetNextCmd();
  const std::size_t GetCmdQueueSize();
  void DetachLastCmd();
  int DoCmd();
  PLC_EXEC_ENUM CheckPostCondition();

  CfgSubscriber cfg_subscriber_;
  std::vector<DelayCfg> delay_cfg_;
  std::vector<FollowerCfg> follower_cfg_;
  std::vector<GasCfg> gas_cfg_;
  
  Gas *gas_;
  Follower *follower_;
  IODevice *output_;

  int current_layer_;
  JobSeeker job_seeker_;
  JobLoader job_loader_;

  double delay_timeout_;
  double delay_left_;
  int DelayCommand(double time);
  int BlowDelayCommand(int craft_level);
  int StayCommand(int craft_level);

  int OpenGas(int craft_level);
  int CloseGas(int craft_level);
  int SetPressure(int craft_level);

  int FollowTo(int craft_level);
  int IncrFollowTo(int craft_level);
  int LiftTo();

};

#endif
