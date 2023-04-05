#include <iostream>

#include "cppserver.h"

namespace core = cppserver_core;

int main() {
  core::Socket* sock = new core::Socket();
  sock->Connect("127.0.0.1", 8888);

  core::Connection* conn = new core::Connection(nullptr, sock);
  while (true) {
    conn->GetLineWriteBuffer();
    conn->Write();
    if (conn->GetState() == core::Connection::State::Closed) {
      conn->Close();
      std::cout << "connection closed" << std::endl;
      break;
    }
    conn->Read();
    std::cout << "message from server: " << conn->ReadBuffer() << std::endl;
  }

  delete sock;
  delete conn;
  return 0;
}