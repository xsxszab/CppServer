#include <unistd.h>

#include "logger.h"

namespace log = cppserver_logger;

int main() {
  log::Logger* logger = log::Logger::GetInstance();
  logger->SetLevel(log::LogLevel::Level::INFO);
  logger->AddAppender(std::make_shared<log::FileAppender>(
      "/home/xsxsz/cppserver/log/log.txt", 1000));

  LOG_INFO("info message");
  LOG_DEBUG("debug message");

  return 0;
}
