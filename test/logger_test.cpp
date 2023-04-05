#include "logger.h"

namespace log = cppserver_logger;

int main() {
  log::Logger logger("test_logger");
  logger.SetLevel(log::LogLevel::Level::INFO);
  logger.AddAppender(
      std::shared_ptr<log::StdoutAppender>(new log::StdoutAppender()));
  return 0;
}