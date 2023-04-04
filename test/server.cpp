#include <cstring>
#include <iostream>

#include "connection.h"
#include "event_loop.h"
#include "server_class.h"

void connectCallback(Connection* conn) {
  conn->read();
  if (conn->getState() == Connection::State::Closed) {
    std::cout << "connection closed" << std::endl;
    conn->close();
    return;
  }
  std::cout << "message from client: " << conn->readBuffer() << std::endl;
  conn->setWriteBuffer(conn->readBuffer());
  conn->write();
}

int main() {
  EventLoop* main_loop = new EventLoop();
  Server* server = new Server(main_loop);

  server->onConnect(connectCallback);

  main_loop->loop();

  delete server;
  delete main_loop;
  return 0;
}
