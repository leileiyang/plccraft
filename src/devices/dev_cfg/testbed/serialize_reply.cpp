#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <zmq.h>

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
  void *responder = zmq_socket(ctx, ZMQ_REP);
  zmq_bind(responder, "tcp://*:5555");

  zmq_msg_t msg;
  int rc = zmq_msg_init(&msg);
  assert(rc == 0);
  rc = zmq_msg_recv(&msg, responder, ZMQ_DONTWAIT);
  if (rc == -1 && errno == EAGAIN) {
    ;
  } else if (rc == -1) {
    ;
  }

  std::string content;
  content = std::string((char *)zmq_msg_data(&msg));
  zmq_msg_close(&msg);

  PlcCmd cmd;
  std::istringstream ifs(content);
  boost::archive::text_iarchive ia(ifs);
  ia >> cmd;

  // reply
  SendMessage(responder, "Received", 0);

  return rc;
}
