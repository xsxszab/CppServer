#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "cppserver.h"

namespace core = cppserver_core;

int main() {
  std::stringstream stream;
  stream << "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: "
            "text\r\n\r\n";

  const char* path = "../../html/index.html";

  std::ifstream t(path);
  stream << t.rdbuf();
  std::string response = stream.str();
  // std::cout << response << std::endl;

  auto server = std::make_unique<core::Server>("0.0.0.0", 8800);

  server->NewConnect([](core::Connection* conn) {
    // std::cout << "new connection fd: " << conn->GetSocket()->GetFd()
    // << std::endl;
  });

  server->OnMessage([&response](core::Connection* conn) {
    // std::cout << conn->ReadBuffer() << std::endl;
    if (conn->GetState() == core::Connection::State::Connected) {
      conn->SetWriteBuffer(response.c_str());
      conn->Write();
    }
  });

  server->Start();

  return 0;
}
