#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "cppserver.h"

namespace core = cppserver_core;
namespace log = cppserver_logger;

int main() {
  log::Logger* logger = log::Logger::GetInstance();
  logger->SetLevel(log::LogLevel::Level::INFO);
  logger->AddAppender(
      std::make_shared<log::FileAppender>("/home/xsxsz/cppserver/log/log.txt"));

  std::stringstream stream;
  stream << "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: "
            "text\r\n\r\n";

  const char* path = "../../html/index.html";

  std::ifstream t(path);
  stream << t.rdbuf();
  std::string response = stream.str();
  // std::cout << response << std::endl;

  auto server = std::make_unique<core::Server>("0.0.0.0", 8000);

  server->SetNewConnectCallback([&logger](core::Connection* conn) {
    std::string msg =
        "new connection fd: " + std::to_string(conn->GetSocket()->GetFd());
    LOG_INFO(msg)
    // std::cout << "new connection fd: " << conn->GetSocket()->GetFd()
    // << std::endl;
  });

  server->SetOnMessageCallback([&response](core::Connection* conn) {
    // std::cout << conn->ReadBuffer() << std::endl;
    if (conn->GetState() == core::Connection::State::Connected) {
      conn->SetWriteBuffer(response.c_str());
      conn->Write();
    }
  });

  server->Start();

  return 0;
}
