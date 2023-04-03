#ifndef BUFFER_H
#define BUFFER_H

#include <string>

class Buffer {
 private:
  std::string buf;

 public:
  Buffer();
  ~Buffer();

  ssize_t size();
  void append(const char* _str);
  void append(const char* _str, int _size);

  void setBuf(const char* _str);
  const char* c_str();

  void clearBuffer();
  void getLine();
};

#endif