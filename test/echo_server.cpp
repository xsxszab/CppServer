#include <cstring>
#include <iostream>

#include "cppserver.h"

namespace core = cppserver_core;
namespace log = cppserver_logger;

int main() {
  log::Logger* logger = log::Logger::GetInstance();
  logger->SetLevel(log::LogLevel::Level::INFO);
  logger->AddAppender(
      std::make_shared<log::FileAppender>("/home/xsxsz/cppserver/log/log.txt"));
  // std::make_shared<log::StdoutAppender>());

  auto server = std::make_unique<core::Server>();

  server->SetNewConnectCallback([&logger](core::Connection* conn) {
    std::cout << "new connection fd: " << conn->GetSocket()->GetFd()
              << std::endl;
    std::string msg =
        "new connection fd: " + std::to_string(conn->GetSocket()->GetFd());
    LOG_INFO(msg)
  });

  server->SetOnMessageCallback([&logger](core::Connection* conn) {
    // std::cout << "message from client: " << conn->ReadBuffer() << std::endl;
    LOG_INFO("read from client")
    if (conn->GetState() == core::Connection::State::Connected) {
      conn->SetWriteBuffer(conn->ReadBuffer());
      conn->Write();
    }
  });

  LOG_INFO("server start")
  server->Start();
  LOG_INFO("server terminated")
  return 0;
}
