#ifndef FLBASECFG_H_
#define FLBASECFG_H_

enum PLC_STATUS {
  UNINIT_STATUS = -1,
  PLC_DONE = 1,
  PLC_EXEC = 2,
  PLC_ERROR = 3
};

enum JOB_OPERATION {
  JOB_NONE,
  JOB_M07,
  JOB_M08,
};

struct PlcJobInfo {
  int operation;
  int job_layer;
  double move_distance;
};

#endif
