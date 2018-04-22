#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <zmq.h>
#include <time.h>

#include "../PlcCfg.h"

int SendMessage(void *socket, const char *content, int flags) {
  zmq_msg_t msg;
  zmq_msg_init_size(&msg, strlen(content));
  memcpy(zmq_msg_data(&msg), content, strlen(content));
  int size = zmq_msg_send(&msg, socket, flags);
  zmq_msg_close(&msg);
  return size;
}

int main() {
  void *ctx = zmq_ctx_new();
  void *requester = zmq_socket(ctx, ZMQ_REQ);
  zmq_connect(requester, "tcp://10.1.0.138:5555");
  int opt = 1000;
  zmq_setsockopt(requester, ZMQ_RCVTIMEO, &opt, sizeof(opt));



  std::ostringstream ofs;
  boost::archive::text_oarchive oa(ofs);
  // PlcCfg class test
  PlcCmd cmd;
  cmd.cmd_id = 100;
  cmd.args = "Hello World";
  oa << cmd;

  clock_t t = clock();
  //const char *content = ofs.str().c_str();
  //int size = SendMessage(requester, content, 0);
  zmq_msg_t msg;
  zmq_msg_init_size(&msg, ofs.str().length());
  memcpy(zmq_msg_data(&msg), ofs.str().c_str(), ofs.str().length());
  int size = zmq_msg_send(&msg, requester, 0);
  zmq_msg_close(&msg);
  if (size == -1) {
    return -1;
  }

  zmq_msg_t ack;
  int rc = zmq_msg_init(&ack);
  rc = zmq_msg_recv(&ack, requester, 0);
  zmq_msg_close(&ack);
  if (rc == -1 && errno == EAGAIN) {
    return -2;
  } else if (rc == -1) {
    return -3;
  } else {
    t = clock() - t;
    double time = ((double)t) / CLOCKS_PER_SEC;
    printf("time:%f\n", time);
    return 0;
  }
  return 0;
}
