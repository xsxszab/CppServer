#include "logger.h"

namespace log = cppserver_logger;

int main() {
  log::Logger* logger = log::Logger::GetInstance();
  logger->SetLevel(log::LogLevel::Level::INFO);
  logger->AddAppender(std::make_shared<log::StdoutAppender>());

  LOG_INFO("info message")
  LOG_DEBUG("debug message")

  return 0;
}
