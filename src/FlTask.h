#ifndef FLTASK_H_
#define FLTASK_H_

#include "devices/gas/Gas.h"
#include <cmd_msg.hh>
#include <nmlmsg.hh>

enum FL_TASK_MODE_ENUM {
 _ FL_TASK_MODE_MANUAL = 1,
  FL_TASK_MODE_AUTO = 2,
  FL_TASK_MODE_MDI = 3
};

class FlTask {
 public:
  int Peek();
  int Execute();

 private:
  Gas *gas_;

  RCS_CMD_MSG *command_;
  int echo_serial_number_;
  FL_TASK_MODE_ENUM mode_;

};

#endif
