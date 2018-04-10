#include "../PlcCfg.h"
#include <zmq.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string.h>

int main() {
  std::ostringstream ofs;
  boost::archive::text_oarchive oa(ofs);
  PlcCmd cmd;
  cmd.cmd_id = 1;
  cmd.args = "This is a test!";
  oa << cmd;

  void *ctx = zmq_ctx_new();
  void *publisher = zmq_socket(ctx, ZMQ_REQ);
  zmq_connect(publisher, "tcp://*:6001");
 
  zmq_msg_t part;
  zmq_msg_init_size(&part, ofs.str().length());
  memcpy(zmq_msg_data(&part), ofs.str().c_str(), ofs.str().length());
  int size = zmq_msg_send(&part, publisher, 0);
  zmq_msg_close(&part);

  zmq_msg_t ack;
  int rc = zmq_msg_init(&ack);
  assert(rc == 0);
  rc = zmq_msg_recv(&ack, publisher, 0);
  if (rc <= 0) {
    if (rc == 0 || errno == EAGAIN) { // no message arrived
      return 0;
    } else {
      return -1; // error happened
    }
  }
  zmq_msg_close(&ack);

  // receiver
  void *reply = zmq_socket(ctx, ZMQ_REP);
  zmq_bind(reply, "tcp://*:6001");
  zmq_msg_t msg;
  zmq_msg_init(&msg);
  rc = zmq_msg_recv(&msg, reply, 0);
  std::string content = std::string((char *)zmq_msg_data(&msg));

  PlcCmd cmd2;
  std::istringstream ifs(content);
  boost::archive::text_iarchive ia(ifs);
  ia >> cmd2;
  std::cout << "cmd id:" << cmd2.cmd_id << std::endl;
  std::cout << "args:" << cmd2.args << std::endl;

  zmq_msg_t ack1;
  zmq_msg_init_size(&ack1, ofs.str().length());
  memcpy(zmq_msg_data(&ack1), "Hello", 5);
  size = zmq_msg_send(&ack1, publisher, 0);
  zmq_msg_close(&ack1);

  return 0;
}
