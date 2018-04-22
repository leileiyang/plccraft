#include "JobSeeker.h"

#include <string.h>


bool JobSeeker::Open(const char *file_name) {
  current_position_.x = 0;
  current_position_.y = 0;
  current_position_.z = 0;
  file_name_ = std::string(file_name);
  return ReOpen();
}

bool JobSeeker::ReOpen() {
  if((fp_ = fopen(file_name_.c_str(), "w")) == NULL) {
    return false;
  }
  current_position_.x = 0;
  current_position_.y = 0;
  current_position_.z = 0;
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
  current_position_.x = 0;
  current_position_.y = 0;
  current_position_.z = 0;
}



PlcJobInfo JobSeeker::GetPlcJobInfo(int motion_line) {
  LocateToGivenLine(motion_line);
  return SeekNextJobOperation();
}

void JobSeeker::LocateToGivenLine(int line) {
  if (!fp_) {
    return;
  }
  char buf[256] = {0};
  for (int i = current_line_; i < line; i++) {
    fgets(buf, 256, fp_);
    if (IsCuttingCurve(buf)) {
      current_position_ = ExtractPosition(buf);
    }
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
    memset(buf, 0, 256);
  }
  if (job_info.operation == JOB_M08) {
    job_info.move_distance = PeekNextMovingDistance();
  }
  return job_info;
}

double JobSeeker::PeekNextMovingDistance() {
  int current_file_position = ftell(fp_);
  double distance = -1;
  char buf[256] = {0};
  while (fgets(buf, 256, fp_)) {
    if (IsMoving(buf)) {
      Point end_point = ExtractPosition(buf);
      distance = PointsDistance(current_position, end_point);
      break;
    } else if (IsCuttingCurve(buf)) {
      break;
    } else if (IsM02(buf)) {
      break;
    }
  }
  fseek(fp_, current_file_position, SEEK_SET);
  return distance;
}
