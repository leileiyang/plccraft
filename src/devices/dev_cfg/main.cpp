#include "GasCfg.h"
#include <zmq.h>
#include <pthread.h>
#include <sstream>
#include <string.h>

static void *subscriber_thread(void *ctx) {
  void *subscriber = zmq_socket(ctx, ZMQ_SUB);
  zmq_connect(subscriber, "tcp://localhost:6001");
  zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "GAS", strlen("GAS"));
  int count = 0;
  while (count < 5) {
    int part_no = 0;
    do {
      sleep(0.1);
      zmq_msg_t part;
      int rc = zmq_msg_init(&part);
      assert(rc == 0);
      rc = zmq_msg_recv(&part, subscriber, ZMQ_DONTWAIT);
      if (rc < 0) {
        break;
      }
      part_no++;
      if (part_no == 2) {
        std::string buffer((char *)zmq_msg_data(&part));
        std::istringstream ifs(buffer);
        boost::archive::text_iarchive ia(ifs);
        GasCfg re_cfg;
        ia >> re_cfg;
        re_cfg.show();
        count++;
      }
      zmq_msg_close(&part);
      if (!zmq_msg_more(&part)) {
        break;
      }
    } while (true);
   }
  zmq_close(subscriber);
}

static void *publisher_thread(void *ctx) {
  void *publisher = zmq_socket(ctx, ZMQ_PUB);
  zmq_bind(publisher, "tcp://*:6000");
  int i = 0;
  std::ostringstream ofs;
  boost::archive::text_oarchive oa(ofs);
  GasCfg gas_cfg;
  gas_cfg.gas_.clear();
  gas_cfg.pressure_.clear();
  for (int i = 0; i < CRAFT_LEVELS; i++) {
    gas_cfg.gas_.push_back((GasType)i);
    gas_cfg.pressure_.push_back(10.0 + i);
  }
  oa << gas_cfg;



  while (1) {
    zmq_msg_t message;
    zmq_msg_init_size(&message, strlen("GAS"));
    memcpy(zmq_msg_data(&message), "GAS", strlen("GAS"));
    int size = zmq_msg_send(&message, publisher, ZMQ_SNDMORE);
    zmq_msg_close(&message);

    zmq_msg_t part;
    zmq_msg_init_size(&part, ofs.str().length());
    memcpy(zmq_msg_data(&part), ofs.str().c_str(), ofs.str().length());
    size = zmq_msg_send(&part, publisher, 0);
    zmq_msg_close(&part);

    sleep(0.1);
  }
}

//static void listener_thread(void *args, zctx_t *ctx, void *pipe) {
//  while (true) {
//    zframe_t *frame = zframe_recv(pipe);
//    if (!frame) {
//      break;
//    }
//    zframe_print(frame, NULL);
//    zframe_destroy(&frame);
//  }
//}

int main() {
  void *ctx = zmq_ctx_new();
  pthread_t a;
  pthread_t b;
  pthread_create(&a, NULL, publisher_thread, ctx);
  pthread_create(&b, NULL, subscriber_thread, ctx);
  //zthread_fork(ctx, publisher_thread, NULL);
  //zthread_fork(ctx, subscriber_thread, NULL);

  void *subscriber = zmq_socket(ctx, ZMQ_XSUB);
  zmq_connect(subscriber, "tcp://localhost:6000");
  void *publisher = zmq_socket(ctx, ZMQ_XPUB);
  zmq_bind(publisher, "tcp://*:6001");
  //void *listener = zthread_fork(ctx, listener_thread, NULL);
  //zmq_proxy(subscriber, publisher, listener);
  zmq_proxy(subscriber, publisher, NULL);
  puts(" interrupted");
  zmq_ctx_destroy(&ctx);
  return 0;
}
