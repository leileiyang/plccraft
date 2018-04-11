#ifndef PLCJOBIMAGE_H_
#define PLCJOBIMAGE_H_

#include <stdio.h>
#include <string>

enum JOB_OPERATION {
  JOB_NONE,
  JOB_M07,
  JOB_M08,
};

struct PlcJobInfo {
  int operation;
  int job_layer;
};

class PlcJobImage {
 public:
  PlcJobImage(): file_name_(""), fp_(NULL), current_line_(0) {}
  bool Open(const char *file_name);
  void Close();
  PlcJobInfo GetPlcJobInfo(int motion_line);
  int GetCurrentLineNo() {
    return current_line_;
  }

 private:
  std::string file_name_; 
  FILE *fp_;
  int current_line_;

  void LocateToGivenLine(int line);
  PlcJobInfo SeekNextJobOperation();
};

#endif // PLCJOBIMAGE_H_
