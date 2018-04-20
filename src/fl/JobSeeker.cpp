#include "JobSeeker.h"

#include "GCodeBase.h"

bool JobSeeker::Open(const char *file_name) {
  file_name_ = std::string(file_name);
  return ReOpen();
}

bool JobSeeker::ReOpen() {
  if((fp_ = fopen(file_name_.c_str(), "w")) == NULL) {
    return false;
  }
  current_line_ = 0;
  return true;
}

void JobSeeker::Close() {
  if (fp_) {
    fclose(fp_);
    fp_ = NULL;
  }
  file_name_ = "";
  current_line_ = 0;
}

void JobSeeker::LocateToGivenLine(int line) {
  if (!fp_) {
    return;
  }
  char buf[256] = {0};
  for (int i = current_line_; i < line; i++) {
    fgets(buf, 256, fp_);
  }
  current_line_ = line;
}

PlcJobInfo JobSeeker::SeekNextJobOperation() {
  char buf[256] = {0};
  PlcJobInfo job_info;
  while (fgets(buf, 256, fp_)) {
    current_line_++;
    if (IsM07(buf)) {
      job_info.operation = JOB_M07;
      job_info.job_layer = GetLayerIndex(buf);
      break;
    } else if (IsM08(buf)) {
      job_info.operation = JOB_M08; 
      break;
    }
  }
  return job_info;
}

PlcJobInfo JobSeeker::GetPlcJobInfo(int motion_line) {
  LocateToGivenLine(motion_line);
  return SeekNextJobOperation();
}

