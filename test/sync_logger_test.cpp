#include "logger.h"

namespace log = cppserver_logger;

int main() {
  log::Logger::ptr logger(new log::Logger("sync_logger"));
  logger->SetLevel(log::LogLevel::Level::INFO);
  logger->AddAppender(
      std::shared_ptr<log::StdoutAppender>(new log::StdoutAppender()));

  LOG_INFO("info message")
  LOG_DEBUG("debug message")

  return 0;
}
