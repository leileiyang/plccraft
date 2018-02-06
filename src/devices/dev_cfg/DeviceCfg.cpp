#include "../gas/Gas.h"
#include <zmq.h>
#include <sstream>
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

};

DeviceCfg::DeviceCfg(): gas_subscriber_(NULL), lhc_subscirber_(NULL),
    context_(NULL) {}

DeviceCfg::~DeviceCfg() {
  zmq_close(gas_subscriber_);
  zmq_close(lhc_subscriber_);
  zmq_ctx_destroy(&content_);
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

int DeviceCfg::ZmqRecvx(void *socket, std::string &identify, std::string &layer,
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
      identity = std::string buffer((char *)zmq_msg_data(&part));
    } else if (part_no == 2) {
      layer = std::string buffer((char *)zmq_msg_data(&part));
    } else if (part_no == 3) {
      content = std::string buffer((char *)zmq_msg_data(&part));
    }
    zmq_msg_close(&part);
    if (!zmq_msg_more(&part)) {
      break;
    }
  } while (true);
  return part_no;
}

int DeviceCfg::UpdateGasCfg(Gas &gas) {
  char *identify = NULL;
  char *layer_str = NULL,
  char *content = NULL;
  int rc = zstr_recvx(gas_subscriber_, &identify, &layer_str, &content, NULL);
  assert(rc >= 0);

  std::istringstream ifs;
  ifs << content;
  boost::archive::text_iarchive ia(ifs);
  int layer = atoi(layer_str);
  assert(layer < gas.gas_cfg_.size());
  ia >> gas.gas_cfg_[layer];
  gas.gas_cfg_[layer].show();
  free(identify);
  free(layer_str);
  free(content);
  return 0;

}
