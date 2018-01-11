/*************************************
 * 1.Peek a message
 * 2.Execute message
 * 3.Update Status
 * ***********************************/

#include "FlTask.h"

int FlTask::PlcTaskExecute() {
  if (exec_state_ == FL_TASK_EXEC_DONE) { 
    switch (plc_craft_.IssueCmd()) {
      case PLC_ERROR:
        exec_state_ = FL_TASK_EXEC_ERROR;
        break;
      case PLC_EXEC:
        exec_state_ = FL_TASK_EXEC_WAITING_FOR_PLC;
        break;
      case PLC_DONE:
      default:
        exec_state_ = FL_TASK_EXEC_DONE;
        break;
    }
  } else if (exec_state_ == FL_TASK_EXEC_WAITING_FOR_PLC) {
    if (plc_craft_.status_ == PLC_DONE) {
      exec_state_ = FL_TASK_EXEC_DONE;
    } else if (plc_craft_.status_ == PLC_ERROR) {
      exec_state_ = FL_TASK_EXEC_ERROR;
    }
  } else if (exec_state_ == FL_TASK_EXEC_WAITING_FOR_MK) {

  }
}

int FlTask::Execute() {
  NMLTYPE type;
  int retval = 0;

  if (command_->serial_number != echo_serial_number_) {
    type = command_->type;
  } else {
    type = 0;
  }

  switch (mode_) {
    case EMC_TASK_MODE_AUTO:
      switch (type) {
        case 0:
          // no command
          if (!paused_) {
            if (emc_status_->task.task_paused) {
              //calling the plc craft
            }
          }
          break;
          // new command
          // plc command
          // mk command
        default:
          break;
      }
      break;
    case EMC_TASK_MODE_MANUAL:
      break;
    default:
      break;
  }

  return retval;
}


void FlTask::UpdateStatus() {
  // update plccraft
  plc_craft_.Update();
  // update machinekit

}
