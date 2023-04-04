#include <iostream>

#include "connection.h"
#include "socket_class.h"

int main() {
  Socket* sock = new Socket();
  sock->connect("127.0.0.1", 8888);

  Connection* conn = new Connection(nullptr, sock);
  while (true) {
    conn->getLineWriteBuffer();
    conn->write();
    if (conn->getState() == Connection::State::Closed) {
      conn->close();
      std::cout << "connection closed" << std::endl;
      break;
    }
    conn->read();
    std::cout << "message from server: " << conn->readBuffer() << std::endl;
  }

  delete sock;
  delete conn;
  return 0;
}