#ifndef DEVICECFG_H_
#define DEVICECFG_H

#include <string>

class Gas;
class Follower;

class DeviceCfg {
 public:
  DeviceCfg();
  ~DeviceCfg();
  int InitCfgSocket();
  int UpdateGasCfg(Gas &gas);
  int UpdateFollowerCfg(Follower &follower);
  int AckAnyReceived();

 private:
  void *gas_subscriber_;
  void *lhc_subscriber_;
  void *ack_responder_;
  void *context_;
  bool received_something_;

  int ZmqRecvx(void *socket, std::string &identify, std::string &layer,
      std::string &content);

};

#endif
