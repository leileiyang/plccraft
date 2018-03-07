#include "DeviceCfg.h"
#include <zmq.h>
#include <pthread.h>
#include <sstream>
#include <string.h>
#include <stdio.h>

int main() {
  DeviceCfg dev_cfg;
  std::vector<GasCfg> gas(18);
  std::vector<FollowerCfg> follower(18);
  if (dev_cfg.InitCfgSocket()) {
    printf("Init DeviceCfg failed!\n");
  }
  while (1) {
    dev_cfg.UpdateGasCfg(gas);
    dev_cfg.UpdateFollowerCfg(follower);
    dev_cfg.AckAnyReceived();
  }
  return 0;
}
