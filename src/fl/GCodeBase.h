#ifndef GCODEBASE_H_
#define GCODEBASE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct Point {
  double x;
  double y;
  double z;
};

int IsCmd(const char *buf, char cmd_type, char cmd_index);

int IsMCmd(const char *buf, char c);

int IsM07(const char *buf);

int IsM08(const char *buf);

int IsM02(const char *buf);

int IsMoving(const char *buf);

int IsCuttingCurve(const char *buf);

int GetLayerIndex(const char *buf);

Point ExtractPosition(const char *buf);

double PointsDistance(const Point &p1, const Point &p2);

#ifdef __cplusplus
}
#endif

#endif // GCODEBASE_H_
