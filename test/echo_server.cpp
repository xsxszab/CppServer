#include <cstring>
#include <iostream>

#include "cppserver.h"

using namespace cppserver_core;

int main() {
  EventLoop* main_loop = new EventLoop();
  Server* server = new Server(main_loop);

  server->NewConnect([](Connection* conn) {
    std::cout << "new connection fd: " << conn->GetSocket()->GetFd()
              << std::endl;
  });

  server->OnMessage([](Connection* conn) {
    std::cout << "message from client: " << conn->ReadBuffer() << std::endl;
    if (conn->GetState() == Connection::State::Connected) {
      conn->SetWriteBuffer(conn->ReadBuffer());
      conn->Write();
    }
  });

  main_loop->Loop();

  delete server;
  delete main_loop;
  return 0;
}
