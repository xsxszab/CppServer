#ifndef BUFFER_H
#define BUFFER_H

#include <string>

#include "marcos.h"

namespace cppserver_core {

class Buffer {
 private:
  std::string buf;

 public:
  Buffer();
  ~Buffer();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Buffer)

  ssize_t size();
  void append(const char* _str);
  void append(const char* _str, int _size);

  void setBuf(const char* _str);
  const char* c_str();

  void clearBuffer();
  void getLine();
};

}  // namespace cppserver_core

#endif