#include "logger.h"

namespace cppserver_logger {
void Logger::AddAppender(Appender::ptr appender) {
  if (!appender->GetFormatter()) {  // if the appender does not has a formatter
                                    // yet, use the default formatter
    appender->SetFormatter(formatter_);
  }
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

// Logger::ptr Logger::GetInstance() {
//   static Logger logger;
//   Logger::ptr log_ptr;
//   log_ptr.reset(&logger);
// }

Logger::Logger(const std::string& name)
    : name_(name), level_(LogLevel::Level::INFO) {
  formatter_.reset(new Formatter("%d [%p] %f %l %m %n"));
}

void Logger::Log(LogLevel::Level level, LogEvent::ptr event) {
  if (level >= level_) {
    for (auto& appender : appenders_) {
      appender->Log(level, event);
    }
  }
}

void Logger::info(LogEvent::ptr event) { Log(LogLevel::INFO, event); }

void Logger::debug(LogEvent::ptr event) { Log(LogLevel::DEBUG, event); }

void Logger::warn(LogEvent::ptr event) { Log(LogLevel::WARN, event); }

void Logger::error(LogEvent::ptr event) { Log(LogLevel::ERROR, event); }

void Logger::fatal(LogEvent::ptr event) { Log(LogLevel::FATAL, event); }

void StdoutAppender::Log(LogLevel::Level level, LogEvent::ptr event) {
  if (level >= level_) {
    std::cout << formatter_->format(level, event);
  }
}

FileAppender::FileAppender(const std::string& file_name, size_t max_queue_size)
    : file_name_(file_name), max_queue_size_(max_queue_size) {
  cppserver_core::Errif(max_queue_size_ == 0, "invalid queue size");

  writer_thread_ = std::thread(&FileAppender::ConsumeLog, this);
  // writer_thread_.detach();

  file_stream_.open(file_name_);
}

FileAppender::~FileAppender() {
  Log(LogLevel::Level::OTHER,
      LogEvent::ptr());  // notify writer thread that logger has terminted
  if (writer_thread_.joinable()) {
    writer_thread_.join();
  }
  file_stream_.close();
}

void FileAppender::Log(LogLevel::Level level, LogEvent::ptr event) {
  std::unique_lock<std::mutex> lock(mtx_);
  while (queue_.size() >= max_queue_size_) {
    cv_not_full_.wait(lock);
  }
  queue_.push(std::make_pair(level, event));
  cv_not_empty_.notify_one();
}

bool FileAppender::Reopen() {
  std::lock_guard<std::mutex> lock(file_mtx_);
  if (file_stream_) {
    file_stream_.close();
  }

  file_stream_.open(file_name_);

  return !!file_stream_;
}

void FileAppender::ConsumeLog() {
  file_stream_ << "log start" << std::endl;
  while (true) {
    std::unique_lock<std::mutex> lock(mtx_);
    while (queue_.empty()) {
      cv_not_empty_.wait(lock);
    }
    auto event_pair = queue_.front();

    queue_.pop();
    cv_not_full_.notify_one();
    lock.unlock();

    auto level = event_pair.first;
    if (level == LogLevel::Level::OTHER) {
      break;
    }
    auto event = event_pair.second;

    {
      std::lock_guard<std::mutex> lock(file_mtx_);

      if (level >= level_) {
        file_stream_ << formatter_->format(level, event);
      }
    }
  }
  file_stream_ << "log end" << std::endl;
}

Appender::~Appender() {}

Formatter::ptr Appender::GetFormatter() const { return formatter_; }

void Appender::SetFormatter(Formatter::ptr _formatter) {
  formatter_ = _formatter;
}

Formatter::Formatter(const std::string& pattern) : pattern_{pattern} {
  Parse();
}

std::string Formatter::format(LogLevel::Level level, LogEvent::ptr event) {
  std::stringstream ss;
  for (auto& item : format_items_) {
    item->format(ss, level, event);
  }
  return ss.str();
}

const std::unordered_map<char, std::function<FormatItem::ptr()>>
    Formatter::format_item_maps_ = {
        {'m', []() { return FormatItem::ptr(new MessageFormatItem()); }},
        {'p', []() { return FormatItem::ptr(new LevelFormatItem()); }},
        {'r', []() { return FormatItem::ptr(new MsElapseFormatItem()); }},
        {'c', []() { return FormatItem::ptr(new FileNameFormatItem()); }},
        {'t', []() { return FormatItem::ptr(new ThreadIdFormatItem()); }},
        {'n', []() { return FormatItem::ptr(new NewLineFormatItem()); }},
        {'d', []() { return FormatItem::ptr(new TimeFormatItem()); }},
        {'f', []() { return FormatItem::ptr(new FileNameFormatItem()); }},
        {'l', []() { return FormatItem::ptr(new LineNumFormatItem()); }},
};

void Formatter::Parse() {
  std::string nstr;
  size_t i = 0;
  while (i < pattern_.size()) {
    if (pattern_[i] != '%') {
      nstr += pattern_[i++];
      continue;
    }

    if (i + 1 >= pattern_.size()) {
      break;
    }
    if (pattern_[i + 1] == '%') {
      nstr += '%';
      i += 2;
      continue;
    }
    if (!nstr.empty()) {
      format_items_.push_back(FormatItem::ptr(new StringFormatItem(nstr)));
      nstr.clear();
    }

    auto it = format_item_maps_.find(pattern_[i + 1]);
    if (it != format_item_maps_.end()) {
      format_items_.push_back(it->second());
    } else {
      format_items_.push_back(
          FormatItem::ptr(new StringFormatItem("<wrong format>")));
      error_ = true;
    }
    i += 2;
  }

  if (!nstr.empty()) {
    format_items_.push_back(FormatItem::ptr(new StringFormatItem(nstr)));
  }
}

LogEvent::LogEvent(const char* file_name, int32_t line_num, uint32_t ms_elapse,
                   uint32_t thread_id, uint64_t time,
                   const std::string& message)
    : file_name_{file_name},
      line_num_{line_num},
      ms_elapse_{ms_elapse},
      thread_id_{thread_id},
      time_{time},
      msg_{message} {}

const char* LogEvent::GetFileName() const { return file_name_; }

int32_t LogEvent::GetLineNum() const { return line_num_; }

uint32_t LogEvent::GetMsElapse() const { return ms_elapse_; }

uint32_t LogEvent::GetThreadId() const { return thread_id_; }

uint64_t LogEvent::GetTime() const { return time_; }

const std::string& LogEvent::GetMessage() const { return msg_; }

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
  out << "ms" << event->GetMsElapse();
}

void ThreadIdFormatItem::format(std::ostream& out, LogLevel::Level level,
                                LogEvent::ptr event) {
  out << "id" << event->GetThreadId();
}

void LineNumFormatItem::format(std::ostream& out, LogLevel::Level level,
                               LogEvent::ptr event) {
  out << event->GetLineNum();
}

void FileNameFormatItem::format(std::ostream& out, LogLevel::Level level,
                                LogEvent::ptr event) {
  out << event->GetFileName();
}

TimeFormatItem::TimeFormatItem() {}

void TimeFormatItem::format(std::ostream& out, LogLevel::Level level,
                            LogEvent::ptr event) {
  out << event->GetTime();
}

void NewLineFormatItem::format(std::ostream& out, LogLevel::Level level,
                               LogEvent::ptr event) {
  out << std::endl;
}

StringFormatItem::StringFormatItem(const std::string& _str) : str_{_str} {}

void StringFormatItem::format(std::ostream& out, LogLevel::Level level,
                              LogEvent::ptr event) {
  out << str_;
}

}  // namespace cppserver_logger
