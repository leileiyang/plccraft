#ifndef JOBSEEKER_H_
#define JOBSEEKER_H_ 

#include <stdio.h>
#include <string>

#include "GCodeBase.h"

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

class JobSeeker {
 public:
  JobSeeker(): file_name_(""), fp_(NULL), current_line_(0) {}
  bool Open(const char *file_name);
  bool ReOpen();
  void Close();
  PlcJobInfo GetPlcJobInfo(int motion_line);
  int GetCurrentLineNo() {
    return current_line_;
  }

 private:
  std::string file_name_; 
  FILE *fp_;
  int current_line_;
  Point current_position_;

  void LocateToGivenLine(int line);
  PlcJobInfo SeekNextJobOperation();
  double PeekNextMovingDistance();
};

#endif // JOBSEEKER_H_
