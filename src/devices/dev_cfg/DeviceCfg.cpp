#include "DeviceCfg.h"

#include <zmq.h>
#include <sstream>
#include <cstdlib>
#include <cstring>


DeviceCfg::DeviceCfg(): gas_subscriber_(NULL), lhc_subscriber_(NULL),
    context_(NULL), received_something_(false) {}

DeviceCfg::~DeviceCfg() {
  zmq_close(gas_subscriber_);
  zmq_close(lhc_subscriber_);
  zmq_close(ack_responder_);
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
  zmq_setsockopt(lhc_subscriber_, ZMQ_SUBSCRIBE, "LHC", strlen("LHC"));

  // ack responder
  ack_responder_ = zmq_socket(context_, ZMQ_REQ);
  zmq_connect(ack_responder_, "tcp://localhost:6000");

  if (gas_subscriber_ && lhc_subscriber_ && ack_responder_) {
    return 0;
  }
  return -1;
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

int DeviceCfg::UpdateGasCfg(std::vector<GasCfg> &gas_cfg) {
  std::string identity;
  std::string layer_str;
  std::string content;
  int rc = ZmqRecvx(gas_subscriber_, identity, layer_str, content);

  if (rc == 3) {
    std::istringstream ifs(content);
    boost::archive::text_iarchive ia(ifs);
    int layer = atoi(layer_str.c_str());
    assert(layer < gas_cfg.size());
    ia >> gas_cfg[layer];
    gas_cfg[layer].Show();
    received_something_ = true;
  } else if (rc < 0) {
    return -1;
  } else if (rc != 0) {
    received_something_ = true;
  }
  return 0;
}

int DeviceCfg::UpdateFollowerCfg(std::vector<FollowerCfg> &follower_cfg) {
  std::string identity;
  std::string layer_str;
  std::string content;

  int rc = ZmqRecvx(lhc_subscriber_, identity, layer_str, content);

  if (rc == 3) {
    std::istringstream ifs(content);
    boost::archive::text_iarchive ia(ifs);
    int layer = atoi(layer_str.c_str());
    assert(layer < follower_cfg.size());
    ia >> follower_cfg[layer];
    follower_cfg[layer].Show();
    received_something_ = true;
  } else if (rc < 0) {
    return -1;
  } else if (rc != 0) {
    received_something_ = true;
  }
  return 0;
}

int DeviceCfg::AckAnyReceived() {
  if (received_something_) {
    received_something_ = false;
    zmq_msg_t reply;
    zmq_msg_init_size(&reply, strlen("Received"));
    memcpy(zmq_msg_data(&reply), "Received", strlen("Received"));
    int rc = zmq_msg_send(&reply, ack_responder_, 0);
    return rc;
  }
  return 0;
}
