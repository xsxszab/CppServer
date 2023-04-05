#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace cppserver_logger {

class Logger;

class LogLevel {
 public:
  enum Level {
    INFO = 1,
    DEBUG = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
  };

  static const char* ToString(Level level);
};

class LogEvent {
 public:
  using ptr = std::shared_ptr<LogEvent>;
  LogEvent() = default;

  const char* GetFileName() const;
  int32_t GetLineNum() const;
  uint32_t GetMsElapse() const;
  uint32_t GetThreadId() const;
  uint64_t GetTime() const;
  std::string GetMessage() const;

 private:
  const char* file_name_{nullptr};
  int32_t line_num_{0};
  uint32_t ms_elapse_{0};
  uint32_t thread_id_{0};
  uint64_t time_{0};
  std::string message_;
};

class FormatItem {
 public:
  using ptr = std::shared_ptr<FormatItem>;
  FormatItem(const std::string& format = "") {}
  virtual ~FormatItem(){};

  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) = 0;
};

class MessageFormatItem : public FormatItem {
 public:
  MessageFormatItem(const std::string& _str = "") : FormatItem(_str) {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class LevelFormatItem : public FormatItem {
 public:
  LevelFormatItem(const std::string& _str = "") : FormatItem(_str) {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class MsElapseFormatItem : public FormatItem {
 public:
  MsElapseFormatItem(const std::string& _str = "") : FormatItem(_str) {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class ThreadIdFormatItem : public FormatItem {
 public:
  ThreadIdFormatItem(const std::string& _str = "") : FormatItem(_str) {}
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
  NewLineFormatItem(const std::string& _str = "") : FormatItem(_str) {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class LineNumFormatItem : public FormatItem {
 public:
  LineNumFormatItem(const std::string& _str = "") : FormatItem(_str) {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class FileNameFormatItem : public FormatItem {
 public:
  FileNameFormatItem(const std::string& _str = "") : FormatItem(_str) {}
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;
};

class TimeFormatItem : public FormatItem {
 public:
  TimeFormatItem(const std::string& format = "%Y:%m:%D %H:%M:%S");
  virtual void format(std::ostream& out, LogLevel::Level level,
                      LogEvent::ptr event) override;

 private:
  std::string time_format_;
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
};

class Appender {
 public:
  using ptr = std::shared_ptr<Appender>;

  virtual ~Appender();
  virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                   LogEvent::ptr event) = 0;

  Formatter::ptr GetFormatter() const;
  void SetFormatter(Formatter::ptr _formatter);

 protected:
  LogLevel::Level level_;
  Formatter::ptr formatter_;
};

class StdoutAppender : public Appender {
 public:
  using ptr = std::shared_ptr<StdoutAppender>;
  virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                   LogEvent::ptr event) override;

 private:
};

class FileAppender : public Appender {
 public:
  FileAppender(const std::string& file_name);
  using ptr = std::shared_ptr<FileAppender>;
  virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                   LogEvent::ptr event) override;

  // if the file is already opened, close it and open again
  // success -> true, fail -> false
  bool Reopen();

 private:
  std::string file_name_;
  std::ofstream file_stream_;
};

class Logger : public std::enable_shared_from_this<Logger> {
 public:
  using ptr = std::shared_ptr<Logger>;

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

}  // namespace cppserver_logger

#endif