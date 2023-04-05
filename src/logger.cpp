#include "logger.h"

namespace cppserver_core {
void Logger::AddAppender(Appender::ptr appender) {
  appenders_.push_back(appender);
}

void Logger::DelAppender(Appender::ptr appender) {
  for (auto it = appenders_.begin(); it != appenders_.end(); it++) {
    if (*it == appender) {
      appenders_.erase(it);
      break;
    }
  }
}

LogLevel::Level Logger::GetLevel() const { return level_; }

void Logger::SetLevel(LogLevel::Level level) { level_ = level; }

std::string Logger::GetName() const { return name_; }

void Logger::SetName(const std::string& name) { name_ = name; }

Logger::Logger(const std::string& name) : name_(name) {}

void Logger::Log(LogLevel::Level level, LogEvent::ptr event) {
  if (level >= level_) {
    for (auto& appender : appenders_) {
      appender->Log(std::make_shared<Logger>(this), level, event);
    }
  }
}

void Logger::info(LogLevel::Level level) {}

void Logger::debug(LogLevel::Level level) {}

void Logger::warn(LogLevel::Level level) {}

void Logger::error(LogLevel::Level level) {}

void Logger::fatal(LogLevel::Level level) {}

void StdoutAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                         LogEvent::ptr event) {
  if (level >= level_) {
    std::cout << formatter_->format(level, event);
  }
}

FileAppender::FileAppender(const std::string& file_name)
    : file_name_{file_name} {}

void FileAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                       LogEvent::ptr event) {
  if (level >= level_) {
    file_stream_ << formatter_->format(level, event);
  }
}

bool FileAppender::Reopen() {
  if (file_stream_) {
    file_stream_.close();
  }

  file_stream_.open(file_name_);

  return !!file_stream_;
}

Formatter::ptr Appender::GetFormatter() const { return formatter_; }

void Appender::SetFormatter(Formatter::ptr _formatter) {
  formatter_ = _formatter;
}

Formatter::Formatter(const std::string& pattern) : pattern_{pattern} {}

std::string Formatter::format(LogLevel::Level level, LogEvent::ptr event) {
  std::stringstream ss;
  for (auto& item : format_items_) {
    item->format(ss, level, event);
  }
  return ss.str();
}

void Formatter::Parse() {
  // tuple(string, format, type)
  std::vector<std::tuple<std::string, std::string, int> > vec;
  std::string nstr;
  for (size_t i = 0; i < pattern_.size(); i++) {
    if (pattern_[i] != '%') {
      nstr += pattern_[i];
      continue;
    }

    if (i + 1 < pattern_.size()) {
      if (pattern_[i + 1] == '%') {
        nstr += '%';
      }
      i++;
      continue;
    }

    size_t pos = i + 1;
    int fmt_status = 0;
    size_t fmt_begin = 0;
    std::string str;
    std::string fmt;
    while (pos < pattern_.size()) {
      if (pattern_[pos] == ' ') {
        break;
      }
      if (fmt_status == 0) {
        if (pattern_[pos] == '{') {
          str = pattern_.substr(i + 1, pos - i - 1);
          fmt_status = 1;
          fmt_begin = pos;
          pos++;
          continue;
        }
      }
      if (fmt_status == 1) {
        if (pattern_[pos] == ')') {
          fmt = pattern_.substr(fmt_begin + 1, pos - fmt_begin - 1);
          fmt_status = 2;
          break;
        }
      }
    }

    if (fmt_status == 0) {
      if (!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
      }
      str = pattern_.substr(i + 1, pos - i - 1);
      vec.push_back(std::make_tuple(str, fmt, 1));
      i = pos;
    } else if (fmt_status == 1) {
      std::cout << "invalid pattern string :" << pattern_ << std::endl;
      vec.push_back(std::make_tuple("<format_error>", fmt, 0));
    } else if (fmt_status == 2) {
      if (!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
      }
      vec.push_back(std::make_tuple(str, fmt, 1));
      i = pos;
    }
  }

  if (!nstr.empty()) {
    vec.push_back(std::make_tuple(nstr, "", 0));
  }
  static std::unordered_map<std::string,
                            std::function<FormatItem::ptr(const std::string&)> >
      s_format_items = {
          {"m",
           [](const std::string& format) {
             return FormatItem::ptr(new MessageFormatItem(format));
           }},
          {"p",
           [](const std::string& format) {
             return FormatItem::ptr(new LevelFormatItem(format));
           }},
          {"r",
           [](const std::string& format) {
             return FormatItem::ptr(new MsElapseFormatItem(format));
           }},
          {"c",
           [](const std::string& format) {
             return FormatItem::ptr(new FileNameFormatItem(format));
           }},
          {"t",
           [](const std::string& format) {
             return FormatItem::ptr(new ThreadIdFormatItem(format));
           }},
          {"n",
           [](const std::string& format) {
             return FormatItem::ptr(new NewLineFormatItem(format));
           }},
          {"d",
           [](const std::string& format) {
             return FormatItem::ptr(new TimeFormatItem(format));
           }},
          {"f",
           [](const std::string& format) {
             return FormatItem::ptr(new FileNameFormatItem(format));
           }},
          {"l",
           [](const std::string& format) {
             return FormatItem::ptr(new LineNumFormatItem(format));
           }},
      };

  for (auto& v : vec) {
    if (std::get<2>(v) == 0) {
      format_items_.push_back(
          FormatItem::ptr(new StringFormatItem(std::get<0>(v))));

    } else {
      auto it = s_format_items.find(std::get<0>(v));
      if (it == s_format_items.end()) {
        format_items_.push_back(FormatItem::ptr(
            new StringFormatItem("<format_error: %" + std::get<0>(v) + ">")));
      } else {
        format_items_.push_back(it->second(std::get<0>(v)));
      }
    }
  }
}

const char* LogEvent::GetFileName() const { return nullptr; }

int32_t LogEvent::GetLineNum() const { return line_num_; }

uint32_t LogEvent::GetMsElapse() const { return ms_elapse_; }

uint32_t LogEvent::GetThreadId() const { return thread_id_; }

uint64_t LogEvent::GetTime() const { return time_; }

std::string LogEvent::GetMessage() const { return message_; }

void MessageFormatItem::format(std::ostream& out, LogLevel::Level level,
                               LogEvent::ptr event) {
  out << event->GetMessage();
}

const char* LogLevel::ToString(Level level) {
  switch (level) {
    case Level::INFO:
      return "INFO";
      break;
    case Level::DEBUG:
      return "DEBUG";
      break;
    case Level::WARN:
      return "WARN";
      break;
    case Level::ERROR:
      return "ERROR";
      break;
    case Level::FATAL:
      return "FATAL";
      break;
    default:
      return "UNKNOWN";
  }
  return "";
}

void LevelFormatItem::format(std::ostream& out, LogLevel::Level level,
                             LogEvent::ptr event) {
  out << LogLevel::ToString(level);
}

void MsElapseFormatItem::format(std::ostream& out, LogLevel::Level level,
                                LogEvent::ptr event) {
  out << event->GetMsElapse();
}

void ThreadIdFormatItem::format(std::ostream& out, LogLevel::Level level,
                                LogEvent::ptr event) {
  out << event->GetThreadId();
}

void LineNumFormatItem::format(std::ostream& out, LogLevel::Level level,
                               LogEvent::ptr event) {
  out << event->GetLineNum();
}

void FileNameFormatItem::format(std::ostream& out, LogLevel::Level level,
                                LogEvent::ptr event) {
  out << event->GetFileName();
}

TimeFormatItem::TimeFormatItem(const std::string& format)
    : time_format_(format) {}

void TimeFormatItem::format(std::ostream& out, LogLevel::Level level,
                            LogEvent::ptr event) {
  out << event->GetTime();
}

void NewLineFormatItem::format(std::ostream& out, LogLevel::Level level,
                               LogEvent::ptr event) {
  out << '\n';
}

StringFormatItem::StringFormatItem(const std::string& _str)
    : str_{_str}, FormatItem(_str) {}

void StringFormatItem::format(std::ostream& out, LogLevel::Level level,
                              LogEvent::ptr event) {
  out << str_;
}

}  // namespace cppserver_core
