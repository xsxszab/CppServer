#include "buffer.h"

#include <cstring>
#include <iostream>

namespace cppserver_core {

Buffer::Buffer() {}

Buffer::~Buffer() {}

void Buffer::getLine() {
  clearBuffer();
  std::getline(std::cin, buf);
}

ssize_t Buffer::size() { return buf.size(); }

const char* Buffer::c_str() { return buf.c_str(); }

void Buffer::append(const char* _str) { buf.append(_str); }

void Buffer::append(const char* _str, int _size) {
  for (int i = 0; i < _size; i++) {
    if (_str[i] == '\0') break;
    buf.push_back(_str[i]);
  }
}

void Buffer::setBuf(const char* _str) {
  buf.clear();
  buf.append(_str);
}

void Buffer::clearBuffer() { buf.clear(); }

}