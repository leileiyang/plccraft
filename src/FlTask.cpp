/*************************************
 * 1.Peek a message
 * 2.Execute message
 * 3.Update Status
 * ***********************************/

#include "FlTask.h"

int FlTask::Execute() {
  NMLTYPE type;
  int retval = 0;

  if (command_->serial_number != echo_serial_number_) {
    type = command_->type;
  } else {
    type = 0;
  }

  switch (mode_) {
    case FL_TASK_MODE_AUTO:
      break;
    case FL_TASK_MODE_MANUAL:
      break;
    default:
      break;
  }

  return retval;
}



