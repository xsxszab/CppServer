#include "http_class.h"

#include "buffer.h"

namespace cppserver_http {
HttpParser::HttpParser(cppserver_core::Buffer::ptr buf) { buf_ = buf; }

HttpCode HttpParser::Parse() {
  int i = 0;
  int j = 0;
}

void HttpParser::SetBuf(cppserver_core::Buffer::ptr buf) { buf_ = buf; }

LineStatus HttpParser::ParseLine() {
  int size = buf_->Size();
  int i = 0;
  char c = '\0';

  while (i < size - 1 && c != '\n') {
    c = buf_->GetBuf()[index_];
    if (c == '\r') {
      if (buf_->GetBuf()[index_ + 1] == '\n') {
      } else {
      }
    }
  }
}

HttpCode HttpParser::ParseRequestLine() {}

HttpCode HttpParser::ParseHeader() {}

HttpCode HttpParser::Parsecontent() {}

}  // namespace cppserver_http