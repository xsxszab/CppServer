#include "logger.h"

namespace log = cppserver_logger;

int main() {
  log::Logger::ptr logger(new log::Logger("test_logger"));
  logger->SetLevel(log::LogLevel::Level::INFO);
  logger->AddAppender(
      std::shared_ptr<log::StdoutAppender>(new log::StdoutAppender()));

  log::LogEvent::ptr event(
      new log::LogEvent(__FILE__, __LINE__, 12, 34, 56, "debug message"));
  logger->Log(log::LogLevel::Level::DEBUG, event);

  return 0;
}
