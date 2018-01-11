#ifndef PLCCRAFT_H_
#define PLCCRAFT_H_

#include <queue>

#include "devices/gas/Gas.h"
#include "devices/follower/Follower.h"

enum PLC_CMD_ENUM {
  // Gas Command
  OPEN_CUTTING_GAS = 100,
  OPEN_FIRST_GAS = 101,
  OPEN_SECOND_GAS = 102,
  OPEN_THIRD_GAS = 103,

  CLOSE_CUTTING_GAS = 104,
  CLOSE_FIRST_GAS = 105,
  CLOSE_SECOND_GAS = 106,
  CLOSE_THIRD_GAS = 107,

  SET_CUTTING_PRESSURE = 108,
  SET_FIRST_PRESSURE = 109,
  SET_SECOND_PRESSURE = 110,
  SET_THIRD_PRESSURE = 111,

  // Follower Command
  FOLLOW_CUTTING_HEIGHT = 120,
  FOLLOW_FIRST_HEIGHT = 121,
  FOLLOW_SECOND_HEIGHT = 122,
  FOLLOW_THIRD_HEIGHT = 123,

  FIRST_PROGRESSIVE = 124,
  SECOND_PROGRESSIVE = 125,
  THRED_PROGRESSIVE = 126,

  FOLLOW_LIFT_TO = 127,

  // Laser Command
  OPEN_LASER = 140,
  CLOSE_LASER = 141,
  OPEN_SHUTTER = 142,
  CLOSE_SHUTTER = 143,

  // Delay Command
  CUTTING_DELAY = 150,
  FIRST_DELAY = 151,
  SECOND_DELAY = 152,
  THIRD_DELAY = 153,

  CUTTING_BLOW_DELAY = 154,
  FIRST_BLOW_DELAY = 155,
  SECOND_BLOW_DELAY = 156,
  THIRD_BLOW_DELAY = 157,

};

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
  void AddCmd(PLC_CMD_ENUM command);
  PLC_STATUS IssueCmd();
  void Update();

  int current_layer_;
  PLC_STATUS status_;

 private:
  std::queue<PLC_CMD_ENUM> cmds_;
  PLC_CMD_ENUM cmd_;
  const PLC_CMD_ENUM GetNextCmd();
  const std::size_t GetCmdQueueSize();
  void DetachLastCmd(); 
  int DoCmd();
  PLC_TASK_EXEC_ENUM CheckPostCondition();

  Gas *gas_;
  Follower *follower_;

};

#endif
