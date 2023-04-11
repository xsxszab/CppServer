#include "buffer.h"

#include <cstring>
#include <iostream>

namespace cppserver_core {

Buffer::Buffer() = default;

Buffer::~Buffer() = default;

void Buffer::GetLine() {
  ClearBuffer();
  std::getline(std::cin, buf_);
}

ssize_t Buffer::Size() const { return buf_.size(); }

const char* Buffer::Cstr() const { return buf_.c_str(); }

void Buffer::Append(const char* _str) { buf_.append(_str); }

void Buffer::Append(const char* _str, int _size) {
  for (int i = 0; i < _size; i++) {
    if (_str[i] == '\0') {
      break;
    }
    buf_.push_back(_str[i]);
  }
}

void Buffer::SetBuf(const char* _str) {
  buf_.clear();
  buf_.append(_str);
}

const std::string& Buffer::GetBuf() { return buf_; }

void Buffer::ClearBuffer() { buf_.clear(); }

}  // namespace cppserver_core