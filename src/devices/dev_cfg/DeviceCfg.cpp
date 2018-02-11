#include "../gas/Gas.h"
#include <zmq.h>
#include <sstream>
#include <string>
#include <cstdlib>

class DeviceCfg {
 public:
  DeviceCfg();
  ~DeviceCfg();
  int InitCfgSocket();
  int UpdateGasCfg(Gas &gas);

 private:
  void *gas_subscriber_;
  void *lhc_subscriber_;
  void *context_;

  int ZmqRecvx(void *socket, std::string &identify, std::string &layer,
      std::string &content); 

};

DeviceCfg::DeviceCfg(): gas_subscriber_(NULL), lhc_subscriber_(NULL),
    context_(NULL) {}

DeviceCfg::~DeviceCfg() {
  zmq_close(gas_subscriber_);
  zmq_close(lhc_subscriber_);
  zmq_ctx_destroy(&context_);
}

int DeviceCfg::InitCfgSocket() {
  void *context_ = zmq_ctx_new();
  // gas subscriber socket
  gas_subscriber_ = zmq_socket(context_, ZMQ_SUB);
  zmq_connect(gas_subscriber_, "tcp://localhost:6001");
  zmq_setsockopt(gas_subscriber_, ZMQ_SUBSCRIBE, "GAS", strlen("GAS"));

  // follower subscriber socket
  lhc_subscriber_ = zmq_socket(context_, ZMQ_SUB);
  zmq_connect(lhc_subscriber_, "tcp://localhost:6001");
  zmq_setsockopt(gas_subscriber_, ZMQ_SUBSCRIBE, "LHC", strlen("LHC"));

}

int DeviceCfg::ZmqRecvx(void *socket, std::string &identity, std::string &layer,
    std::string &content) {

  int part_no = 0;
  do {
    zmq_msg_t part;
    int rc = zmq_msg_init(&part);
    assert(rc == 0);
    rc = zmq_msg_recv(&part, socket, ZMQ_DONTWAIT);
    if (rc <= 0) {
      if (rc == 0 || errno == EAGAIN) { // no message arrived
        return 0;
      } else {
        return -1; // error happened
      }
    }
    part_no++;
    if (part_no == 1) {
      identity = std::string((char *)zmq_msg_data(&part));
    } else if (part_no == 2) {
      layer = std::string((char *)zmq_msg_data(&part));
    } else if (part_no == 3) {
      content = std::string((char *)zmq_msg_data(&part));
    }
    zmq_msg_close(&part);
    if (!zmq_msg_more(&part)) {
      break;
    }
  } while (true);
  return part_no;
}

int DeviceCfg::UpdateGasCfg(Gas &gas) {
  std::string identity;
  std::string layer_str;
  std::string content;
  int rc = ZmqRecvx(gas_subscriber_, identity, layer_str, content);

  if (rc == 3) {
    std::istringstream ifs(content);
    boost::archive::text_iarchive ia(ifs);
    int layer = atoi(layer_str.c_str());
    assert(layer < gas.gas_cfg_.size());
    ia >> gas.gas_cfg_[layer];
    gas.gas_cfg_[layer].show();
  } else if (rc < 0) {
    return -1;
  }
  return 0;
}
