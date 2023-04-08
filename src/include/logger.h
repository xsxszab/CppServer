#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <queue>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "utilfunc.h"

namespace cppserver_logger {

uint64_t getTimeStamp() {
  std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>
      tp = std::chrono::time_point_cast<std::chrono::milliseconds>(
          std::chrono::system_clock::now());  // get current time
  std::time_t timestamp =
      tp.time_since_epoch()
          .count();  // calc diff between now and 1970-1-1,00:00
  return static_cast<uint64_t>(timestamp);
}

class Logger;

class LogLevel {
 public:
  enum Level {
    INFO = 1,
    DEBUG = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
    OTHER = 6,  // used by FileAppender exclusively
  };

  static const char* ToString(Level level);
};

class LogEvent {
 public:
  using ptr = std::shared_ptr<LogEvent>;
  LogEvent(const char* file_name, int32_t line_num, uint32_t ms_elapse,
           uint32_t thread_id, uint64_t time, const std::string& message);

  const char* GetFileName() const;
  int32_t GetLineNum() const;
  uint32_t GetMsElapse() const;
  uint32_t GetThreadId() const;
  uint64_t GetTime() const;
  const std::string& GetMessage() const;

 private:
  const char* file_name_{nullptr};
  int32_t line_num_{0};
  uint32_t ms_elapse_{0};
  uint32_t thread_id_{0};
  uint64_t time_{0};
  std::string msg_;
};

class FormatItem {
 public:
  using ptr = std::shared_ptr<FormatItem>;
  FormatItem() {}
  virtual ~FormatItem(){};

  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) = 0;
};

class MessageFormatItem : public FormatItem {
 public:
  MessageFormatItem() {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class LevelFormatItem : public FormatItem {
 public:
  LevelFormatItem() {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class MsElapseFormatItem : public FormatItem {
 public:
  MsElapseFormatItem() {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class ThreadIdFormatItem : public FormatItem {
 public:
  ThreadIdFormatItem() {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class StringFormatItem : public FormatItem {
 public:
  StringFormatItem(const std::string& _str = "");
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;

 private:
  std::string str_;
};

class NewLineFormatItem : public FormatItem {
 public:
  NewLineFormatItem() {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class LineNumFormatItem : public FormatItem {
 public:
  LineNumFormatItem() {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class FileNameFormatItem : public FormatItem {
 public:
  FileNameFormatItem() {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class TimeFormatItem : public FormatItem {
 public:
  TimeFormatItem();
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;

 private:
  std::string time_format_{"%Y:%m:%D %H:%M:%S"};
};

// class for formatting log string
class Formatter {
 public:
  using ptr = std::shared_ptr<Formatter>;

  Formatter(const std::string& pattern);
  std::string format(LogLevel::Level level, LogEvent::ptr event);

 private:
  // parse pattern_ string into several FormatItems, saved in format_items_
  void Parse();

  std::string pattern_;
  std::vector<FormatItem::ptr> format_items_;
  bool error_{false};

  static const std::unordered_map<char, std::function<FormatItem::ptr()>>
      format_item_maps_;
};

class Appender {
 public:
  using ptr = std::shared_ptr<Appender>;

  Appender() = default;
  virtual ~Appender();
  virtual void Log(LogLevel::Level level, LogEvent::ptr event) = 0;

  Formatter::ptr GetFormatter() const;
  void SetFormatter(Formatter::ptr _formatter);

 protected:
  LogLevel::Level level_;
  Formatter::ptr formatter_;
};

// directly print logs to stdout, sync
class StdoutAppender : public Appender {
 public:
  using ptr = std::shared_ptr<StdoutAppender>;
  virtual void Log(LogLevel::Level level, LogEvent::ptr event) override;
};

// print logs to file, async
class FileAppender : public Appender {
 public:
  FileAppender(const std::string& file_name, size_t max_queue_size);
  ~FileAppender();
  using ptr = std::shared_ptr<FileAppender>;
  virtual void Log(LogLevel::Level level, LogEvent::ptr event) override;

  // if the file is already opened, close it and open again
  // success -> true, fail -> false
  bool Reopen();

 private:
  void ConsumeLog();

  std::string file_name_;
  std::ofstream file_stream_;
  std::queue<std::pair<LogLevel::Level, LogEvent::ptr>> queue_;
  std::thread writer_thread_;
  std::mutex mtx_;
  std::mutex file_mtx_;
  std::condition_variable cv_not_empty_;
  std::condition_variable cv_not_full_;
  size_t max_queue_size_{0};
};

class Logger {
 public:
  using ptr = std::shared_ptr<Logger>;

  // // singleton
  // static ptr GetInstance();

  Logger(const std::string& name = "empty_name");
  void Log(LogLevel::Level level, LogEvent::ptr event);

  void info(LogEvent::ptr event);
  void debug(LogEvent::ptr event);
  void warn(LogEvent::ptr event);
  void error(LogEvent::ptr event);
  void fatal(LogEvent::ptr event);

  void AddAppender(Appender::ptr appender);
  void DelAppender(Appender::ptr appender);

  LogLevel::Level GetLevel() const;
  void SetLevel(LogLevel::Level level);
  std::string GetName() const;
  void SetName(const std::string& name);

 private:
  std::string name_;       // logger name
  LogLevel::Level level_;  // log level, lower level logs will be omitted
  std::list<Appender::ptr> appenders_;
  Formatter::ptr formatter_;  // default formatter
};

#define LOG_INFO(msg)                                                      \
  {                                                                        \
    log::LogEvent::ptr event(new log::LogEvent(                            \
        __FILE__, __LINE__, 0, 0, cppserver_logger::getTimeStamp(), msg)); \
    logger->Log(log::LogLevel::Level::INFO, event);                        \
  }

#define LOG_DEBUG(msg)                                                     \
  {                                                                        \
    log::LogEvent::ptr event(new log::LogEvent(                            \
        __FILE__, __LINE__, 0, 0, cppserver_logger::getTimeStamp(), msg)); \
    logger->Log(log::LogLevel::Level::DEBUG, event);                       \
  }

#define LOG_WARN(msg)                                                      \
  {                                                                        \
    log::LogEvent::ptr event(new log::LogEvent(                            \
        __FILE__, __LINE__, 0, 0, cppserver_logger::getTimeStamp(), msg)); \
    logger->Log(log::LogLevel::Level::WARN, event);                        \
  }

#define LOG_ERROR(msg)                                                     \
  {                                                                        \
    log::LogEvent::ptr event(new log::LogEvent(                            \
        __FILE__, __LINE__, 0, 0, cppserver_logger::getTimeStamp(), msg)); \
    logger->Log(log::LogLevel::Level::ERROR, event);                       \
  }

#define LOG_FATAL(msg)                                                     \
  {                                                                        \
    log::LogEvent::ptr event(new log::LogEvent(                            \
        __FILE__, __LINE__, 0, 0, cppserver_logger::getTimeStamp(), msg)); \
    logger->Log(log::LogLevel::Level::FATAL, event);                       \
  }

}  // namespace cppserver_logger

#endif
