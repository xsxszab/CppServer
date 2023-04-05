#include <cstring>
#include <iostream>

#include "cppserver.h"

namespace core = cppserver_core;

int main() {
  auto server = std::make_unique<core::Server>();

  server->NewConnect([](core::Connection* conn) {
    std::cout << "new connection fd: " << conn->GetSocket()->GetFd()
              << std::endl;
  });

  server->OnMessage([](core::Connection* conn) {
    std::cout << "message from client: " << conn->ReadBuffer() << std::endl;
    if (conn->GetState() == core::Connection::State::Connected) {
      conn->SetWriteBuffer(conn->ReadBuffer());
      conn->Write();
    }
  });

  server->Start();

  return 0;
}
