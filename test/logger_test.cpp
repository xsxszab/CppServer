#include "logger.h"

namespace log = cppserver_logger;

int main() {
  log::Logger::ptr logger(new log::Logger("test_logger"));
  logger->SetLevel(log::LogLevel::Level::INFO);
  logger->AddAppender(
      std::shared_ptr<log::StdoutAppender>(new log::StdoutAppender()));

  LOG_INFO("test message")

  return 0;
}
