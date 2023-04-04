#include <cstring>
#include <iostream>

#include "connection.h"
#include "event_loop.h"
#include "server_class.h"
#include "socket_class.h"

int main() {
  EventLoop* main_loop = new EventLoop();
  Server* server = new Server(main_loop);

  server->newConnect([](Connection* conn) {
    std::cout << "new connection fd: " << conn->getSocket()->getFd()
              << std::endl;
  });

  server->onMessage([](Connection* conn) {
    std::cout << "message from client: " << conn->readBuffer() << std::endl;
    if (conn->getState() == Connection::State::Connected) {
      conn->setWriteBuffer(conn->readBuffer());
      conn->write();
    }
  });

  main_loop->loop();

  delete server;
  delete main_loop;
  return 0;
}
