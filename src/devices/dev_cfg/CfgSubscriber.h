#ifndef DEVICECFG_H_
#define DEVICECFG_H

#include <string>

#include "PlcCfg.h"
#include "GasCfg.h"
#include "FollowerCfg.h"
#include "LaserCfg.h"
#include "DelayCfg.h"

class CfgSubscriber {
 public:
  CfgSubscriber();
  ~CfgSubscriber();
  int InitCfgSocket();
  int UpdateGasCfg(std::vector<GasCfg> &gas_cfg);
  int UpdatePlcCfg(PlcCfg &plc_cfg);
  int UpdateFollowerCfg(std::vector<FollowerCfg> &follower_cfg);
  int AckAnyReceived();

  int PullCommand(PlcCmd &cmd);

 private:
  // data sockets
  void *gas_subscriber_;
  void *lhc_subscriber_;
  void *plc_subscriber_;
  void *ack_responder_;
  // command sockets
  void *responder_;

  void *context_;
  bool received_something_;

  int ZmqRecvx(void *socket, std::string &identify, std::string &layer,
      std::string &content);

};

#endif
