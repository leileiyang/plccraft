#include "FollowerCfg.h"
#include <zmq.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string.h>

int main() {
  void *ctx = zmq_ctx_new();
  void *publisher = zmq_socket(ctx, ZMQ_PUB);
  zmq_connect(publisher, "tcp://localhost:6001");

  void *sync = zmq_socket(ctx, ZMQ_REP);
  zmq_bind(sync, "tcp://*:6000");

  do {
  zmq_msg_t hello;
  zmq_msg_init_size(&hello, strlen("LHC"));
  memcpy(zmq_msg_data(&hello), "LHC", strlen("LHC"));
  int rc = zmq_msg_send(&hello, publisher, 0);
  zmq_msg_close(&hello);
  rc = 0;

  zmq_msg_t reply;
  rc = zmq_msg_init(&reply);
  rc = zmq_msg_recv(&reply, sync, ZMQ_DONTWAIT);
  if (rc >= 0) {
    break;
  }
  } while (1);

  int i = 0;
  std::ostringstream ofs;
  boost::archive::text_oarchive oa(ofs);
  FollowerCfg lhc_cfg;
  lhc_cfg.height_.clear();
  lhc_cfg.incr_enable_.clear();
  lhc_cfg.incr_time_.clear();

  for (int i = 0; i < CRAFT_LEVELS; i++) {
    lhc_cfg.height_.push_back(i);
    lhc_cfg.incr_time_.push_back(10.0 + i);
    lhc_cfg.incr_enable_.push_back(false);
  }
  lhc_cfg.lift_height_ = 7;

  oa << lhc_cfg;

  do {
  i = 0;
  while (i < 5) {
    zmq_msg_t message;
    zmq_msg_init_size(&message, strlen("LHC"));
    memcpy(zmq_msg_data(&message), "LHC", strlen("LHC"));
    int size = zmq_msg_send(&message, publisher, ZMQ_SNDMORE);
    std::cout << "size1: " << size << std::endl;
    zmq_msg_close(&message);

    zmq_msg_t part;
    zmq_msg_init_size(&part, ofs.str().length());
    memcpy(zmq_msg_data(&part), ofs.str().c_str(), ofs.str().length());
    size = zmq_msg_send(&part, publisher, 0);
    std::cout << "size2: " << size << std::endl;
    zmq_msg_close(&part);

    sleep(0.1);
    i++;
  }
  std::cin >> i;
  if (i == 0) {
    break;
  }
  } while (1);
  return 0;
}
