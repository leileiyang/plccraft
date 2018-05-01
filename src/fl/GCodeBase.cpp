#include "GCodeBase.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int IsCmd(const char *buf, char cmd_type, char cmd_index) {
  char cmd = 0; 
  int cmd_value = 0;
  sscanf(buf, "%c%d", &cmd, &cmd_value);

  return cmd == cmd_type && cmd_value == cmd_index;
}

int IsMCmd(const char *buf, char cmd_index) {
  return IsCmd(buf, 'M', cmd_index);
}

int IsM07(const char *buf) {
  return IsMCmd(buf, 7);
}

int IsM08(const char *buf) {
  return IsMCmd(buf, 8);
}

int IsM02(const char *buf) {
  return IsMCmd(buf, 2);
}

int IsMoving(const char *buf) {
  return IsCmd(buf, 'G', 0);
}

int IsCuttingCurve(const char *buf) {
  return IsCmd(buf, 'G', 1) || IsCmd(buf, 'G', 2) || IsCmd(buf, 'G', 3);
}

int GetLayerIndex(const char *buf) {
  // M07 command: M07 (1) 
  int index = 0;
  char m[20] = {0};
  sscanf(buf, "%s (%d)", m, &index);
  return index;
}

Point ExtractPosition(const char *buf) {
  Point point = {0, 0, 0};
  char g[20] = {0};
  sscanf(buf, "%s X%lf Y%lf", g, &point.x, &point.y);
  return point;
}

double PointsDistance(const Point &p1, const Point &p2) {
  return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}
