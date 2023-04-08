#include <unistd.h>

#include "logger.h"

namespace log = cppserver_logger;

int main() {
  log::Logger::ptr logger(new log::Logger("async_logger"));
  logger->SetLevel(log::LogLevel::Level::INFO);
  logger->AddAppender(std::shared_ptr<log::FileAppender>(
      new log::FileAppender("/home/xsxsz/cppserver/log/log.txt", 1000)));

  LOG_INFO("info message");
  LOG_DEBUG("debug message");

  return 0;
}
