#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "cppserver.h"

namespace core = cppserver_core;
namespace http = cppserver_http;
namespace log = cppserver_logger;

int main() {
  log::Logger* logger = log::Logger::GetInstance();
  logger->SetLevel(log::LogLevel::Level::INFO);
  logger->AddAppender(
      std::make_shared<log::FileAppender>("/home/xsxsz/cppserver/log/log.txt"));

  auto server = std::make_unique<core::Server>("0.0.0.0", 8000);

  http::HttpManager http_manager;

  server->SetOnConnectCallback(
      [&logger, &http_manager](std::shared_ptr<core::Connection> conn) {
        http_manager.OnConnection(conn);
      });

  server->SetOnMessageCallback(
      [&http_manager](std::shared_ptr<core::Connection> conn) {
        http_manager.OnMessage(conn);
      });

  server->Start();

  return 0;
}
