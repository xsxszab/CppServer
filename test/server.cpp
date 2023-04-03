#include <cstring>
#include <iostream>

#include "event_loop.h"
#include "server_class.h"

int main() {
  EventLoop* main_loop = new EventLoop();
  Server* server = new Server(main_loop);
  main_loop->loop();

  delete server;
  return 0;
}
