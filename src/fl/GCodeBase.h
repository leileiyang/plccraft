#ifndef GCODEBASE_H_
#define GCODEBASE_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int IsMCmd(const char *buf, char c) {
  if (strlen(buf) < 3) {
    return 0;
  }
  return buf[0] == 'M' && buf[1] == '0' && buf[2] == c;
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

#ifdef __cplusplus
}
#endif

#endif // GCODEBASE_H_
