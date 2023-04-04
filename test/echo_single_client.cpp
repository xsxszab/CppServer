#include <iostream>

#include "cppserver.h"

using namespace cppserver_core;

int main() {
  Socket* sock = new Socket();
  sock->Connect("127.0.0.1", 8888);

  Connection* conn = new Connection(nullptr, sock);
  while (true) {
    conn->GetLineWriteBuffer();
    conn->Write();
    if (conn->GetState() == Connection::State::Closed) {
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