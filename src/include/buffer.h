#ifndef BUFFER_H
#define BUFFER_H

#include <string>

#include "marcos.h"

namespace cppserver_core {

class Buffer {
 private:
  std::string buf_;

 public:
  explicit Buffer();
  ~Buffer();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Buffer)

  ssize_t Size() const;
  void Append(const char* _str);
  void Append(const char* _str, int _size);

  void SetBuf(const char* _str);
  const std::string& GetBuf();
  const char* Cstr() const;

  void ClearBuffer();
  void GetLine();
};

}  // namespace cppserver_core

#endif