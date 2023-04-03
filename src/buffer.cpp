#include <cstring>
#include <iostream>

#include "buffer.h"

Buffer::Buffer() {}

Buffer::~Buffer() {}

void Buffer::getLine() {
  clearBuffer();
  std::getline(std::cin, buf);
}

ssize_t Buffer::size() { return buf.size(); }

const char *Buffer::c_str() { return buf.c_str(); }

void Buffer::append(const char *_str, int _size) {
  for (int i = 0; i < _size; i++) {
    if (_str[i] == '\0')
      break;
    buf.push_back(_str[i]);
  }
}

void Buffer::clearBuffer() { buf.clear(); }
