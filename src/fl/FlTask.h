#ifndef FLTASK_H_
#define FLTASK_H_

#include <queue>

#include <linuxcnc/cmd_msg.hh>
#include <linuxcnc/nmlmsg.hh>
#include <linuxcnc/emc_nml.hh>

#include "PlcCraft.h"

enum FL_TASK_EXEC_ENUM {
    FL_TASK_EXEC_ERROR = 1,
    FL_TASK_EXEC_DONE = 2,
    FL_TASK_EXEC_WAITING_FOR_MK = 3,
    FL_TASK_EXEC_WAITING_FOR_PLC = 4,

};

class FlTask {
 public:
  int Peek();
  int Execute();
  int PlcTaskExecute();
  void UpdateStatus();

 private:
  RCS_CMD_MSG *command_;
  int echo_serial_number_;
  EMC_TASK_MODE_ENUM mode_;
  FL_TASK_EXEC_ENUM exec_state_;
  int paused_;

  // machinekit status
  EMC_STAT *emc_status_;

  // plc execute logic segment
  PlcCraft plc_craft_;


};

#endif
