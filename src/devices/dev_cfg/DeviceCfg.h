#ifndef DEVICECFG_H_
#define DEVICECFG_H

class Gas;
class Follower;

class DeviceCfg {
 public:
  DeviceCfg();
  ~DeviceCfg();
  int InitCfgSocket();
  int UpdateGasCfg(Gas &gas);
  int UpdateFollowerCfg(Follower &follower);

 private:
  void *gas_subscriber_;
  void *lhc_subscriber_;
  void *context_;

  int ZmqRecvx(void *socket, std::string &identify, std::string &layer,
      std::string &content);

};

#endif
