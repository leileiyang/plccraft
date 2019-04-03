#include "CfgSubscriber.h"

#include <zmq.h>
#include <sstream>
#include <cstdlib>
#include <cstring>


CfgSubscriber::CfgSubscriber(): gas_subscriber_(NULL), lhc_subscriber_(NULL),
    plc_subscriber_(NULL), responder_(NULL), context_(NULL), 
    received_cfg_(false), cfg_socket_connected_(false) {}

CfgSubscriber::~CfgSubscriber() {
  zmq_close(gas_subscriber_);
  zmq_close(lhc_subscriber_);
  zmq_close(plc_subscriber_);
  zmq_close(ack_responder_);
  zmq_close(responder_);
  zmq_ctx_destroy(&context_);
}

int CfgSubscriber::InitCfgSocket() {
  void *context_ = zmq_ctx_new();
  // follower subscriber socket
  lhc_subscriber_ = zmq_socket(context_, ZMQ_SUB);
  zmq_connect(lhc_subscriber_, "tcp://10.1.0.165:6001");
  zmq_setsockopt(lhc_subscriber_, ZMQ_SUBSCRIBE, "LHC", strlen("LHC"));

  // gas subscriber socket
  gas_subscriber_ = zmq_socket(context_, ZMQ_SUB);
  zmq_connect(gas_subscriber_, "tcp://10.1.0.165:6001");
  zmq_setsockopt(gas_subscriber_, ZMQ_SUBSCRIBE, "GAS", strlen("GAS"));


  // plc subscriber socket
  plc_subscriber_ = zmq_socket(context_, ZMQ_SUB);
  zmq_connect(plc_subscriber_, "tcp://10.1.0.165:6001");
  zmq_setsockopt(plc_subscriber_, ZMQ_SUBSCRIBE, "PLC", strlen("PLC"));

  // ack responder
  ack_responder_ = zmq_socket(context_, ZMQ_REQ);
  zmq_connect(ack_responder_, "tcp://10.1.0.165:6000");
  
  // task status publisher
  status_publisher_ = zmq_socket(context_, ZMQ_PUB);
  zmq_bind(status_publisher_, "tcp://*:6002");

  // commmand responder socket
  responder_ = zmq_socket(context_, ZMQ_REP);
  zmq_bind(responder_, "tcp://*:5555");

  if (gas_subscriber_ && lhc_subscriber_ && plc_subscriber_&& ack_responder_ &&
      responder_ && status_publisher_) {

    return 0;
  }
  return -1;
}

int CfgSubscriber::ZmqRecvx(void *socket, std::string &identity, std::string &layer,
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

#define UNSERIALIZE_CFG(CFG) \
  std::istringstream ifs(content); \
  boost::archive::text_iarchive ia(ifs); \
  ia >> CFG

int CfgSubscriber::PullCommand(PlcCmd &cmd) {
  zmq_msg_t msg;
  int rc = zmq_msg_init(&msg);
  assert(rc == 0);
  rc = zmq_msg_recv(&msg, responder_, ZMQ_DONTWAIT);
  if (rc == -1 && errno == EAGAIN) {
    return 0;
  } else if (rc == -1) {
    return -1;
  }

  std::string content;
  content = std::string((char *)zmq_msg_data(&msg));
  zmq_msg_close(&msg);

  UNSERIALIZE_CFG(cmd);

  // reply
  return SendMessage(responder_, "Received", 0);
}

int CfgSubscriber::UpdatePlcCfg(PlcCfg &plc_cfg) {
  std::string identity;
  std::string layer_str;
  std::string content;
  int rc = ZmqRecvx(plc_subscriber_, identity, layer_str, content); 
  if (rc == 3) {
    UNSERIALIZE_CFG(plc_cfg);
    plc_cfg.Show();
    received_cfg_ = true;
  } else if (rc < 0) {
    return -1;
  } else if (rc != 0) {
    received_cfg_ = true;
  }
  return 0;
}

int CfgSubscriber::UpdateGasCfg(std::vector<GasCfg> &gas_cfg) {
  std::string identity;
  std::string layer_str;
  std::string content;
  int rc = ZmqRecvx(gas_subscriber_, identity, layer_str, content);

  if (rc == 3) {
    int layer = atoi(layer_str.c_str());
    assert(layer < gas_cfg.size());

    UNSERIALIZE_CFG(gas_cfg[layer]);

    gas_cfg[layer].Show();
    received_cfg_ = true;
  } else if (rc < 0) {
    return -1;
  } else if (rc != 0) {
    received_cfg_ = true;
  }
  return 0;
}

int CfgSubscriber::UpdateFollowerCfg(std::vector<FollowerCfg> &follower_cfg) {
  std::string identity;
  std::string layer_str;
  std::string content;

  int rc = ZmqRecvx(lhc_subscriber_, identity, layer_str, content);

  if (rc == 3) {
    int layer = atoi(layer_str.c_str());
    assert(layer < follower_cfg.size());

    UNSERIALIZE_CFG(follower_cfg[layer]);

    follower_cfg[layer].Show();
    received_cfg_ = true;
  } else if (rc < 0) {
    return -1;
  } else if (rc != 0) {
    received_cfg_ = true;
  }
  return 0;
}

int CfgSubscriber::AckCfgReceived() {
  if (!cfg_socket_connected_) {
    if (received_cfg_) {
      received_cfg_ = false;
      cfg_socket_connected_ = true;
      return SendMessage(ack_responder_, "Received", 0);
    }
  }
  return 0;
}

int CfgSubscriber::UpdateTaskStatus(const TaskStatus &task_status) {
  std::ostringstream ofs;
  boost::archive::text_oarchive oa(ofs);
  oa << task_status;

  const std::string &content = ofs.str();
  return SendMessage(status_publisher_, content.c_str(), 0);
}

int CfgSubscriber::SendMessage(void *socket, const char *content, int flags) {
  zmq_msg_t msg;
  zmq_msg_init_size(&msg, strlen(content));
  memcpy(zmq_msg_data(&msg), content, strlen(content));
  int size = zmq_msg_send(&msg, socket, flags);
  assert(size >= 0);
  zmq_msg_close(&msg);
  return size;
}
