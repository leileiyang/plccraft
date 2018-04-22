#ifndef GCODEBASE_H_
#define GCODEBASE_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

struct Point {
  double x;
  double y;
  double z;
};

int IsCmd(const char *buf, char cmd_type, char cmd_index) {
  char cmd[10] = {0};
  sscanf(buf, "%s", cmd);
  if (strlen(cmd) < 3) {
    return cmd[0] == cmd_type && cmd[1] == cmd_index;
  } else {
    return cmd[0] == cmd_type && cmd[1] == '0' && cmd[2] == cmd_index;
  }
}

int IsMCmd(const char *buf, char c) {
  return IsCmd(buf, 'M', c);
}

int IsM07(const char *buf) {
  return IsMCmd(buf, '7');
}

int IsM08(const char *buf) {
  return IsMCmd(buf, '8');
}

int IsM02(const char *buf) {
  return IsMCmd(buf, '2');
}

int IsMoving(const char *buf) {
  return IsCmd(buf, 'G', '0');
}

int IsCuttingCurve(const char *buf) {
  return IsCmd('G', '1') || IsCmd('G', '2') || IsCmd('G', '3');
}

int GetLayerIndex(const char *buf) {
  // M07 command: M07 U1
  int index = 0;
  char m[20] = {0};
  char u[20] = {0};
  sscanf(buf, "%s %s", m, u);
  if (strlen(u)) {
    index = atoi(&u[1]);
  }
  return index;
}

Point ExtractPosition(const char *buf) {
  Point point = {0, 0, 0};
  char g[20] = {0};
  char x[20] = {0};
  char y[20] = {0};
  sscanf(buf, "%s %s %s", g, x, y);
  if (strlen(x)) {
    if (x[0] == 'X') {
      point.x = atof(&x[1]);
    } else if (x[0] == 'Y') {
      point.y = atof(&x[1]);
    }
  }
  if (strlen(y)) {
    point.y = atof(&y[1]);
  }
  return point;
}

double PointsDistance(const Point &p1, const Point &p2) {
  return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

#ifdef __cplusplus
}
#endif

#endif // GCODEBASE_H_
