#ifndef DEVICECFG_H_
#define DEVICECFG_H

#include <string>

#include "PlcCfg.h"
#include "GasCfg.h"
#include "FollowerCfg.h"
#include "LaserCfg.h"
#include "DelayCfg.h"
#include "TaskStatus.h"

class CfgSubscriber {
 public:
  CfgSubscriber();
  ~CfgSubscriber();
  int InitCfgSocket();
  int UpdateGasCfg(std::vector<GasCfg> &gas_cfg);
  int UpdatePlcCfg(PlcCfg &plc_cfg);
  int UpdateFollowerCfg(std::vector<FollowerCfg> &follower_cfg);
  int UpdateTaskStatus(const TaskStatus &task_status);
  int AckCfgReceived();

  int PullCommand(PlcCmd &cmd);

 private:
  // data sockets
  void *gas_subscriber_;
  void *lhc_subscriber_;
  void *plc_subscriber_;
  void *ack_responder_;
  void *status_publisher_;
  // command sockets
  void *responder_;

  void *context_;
  bool received_cfg_;
  bool cfg_socket_connected_;

  int ZmqRecvx(void *socket, std::string &identify, std::string &layer,
      std::string &content);

  int SendMessage(void *socket, const char *content, int flags);

};

#endif
