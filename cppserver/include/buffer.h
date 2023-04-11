#ifndef BUFFER_H
#define BUFFER_H

#include <memory>
#include <string>

#include "marcos.h"

namespace cppserver_core {

class Buffer {
 public:
  using ptr = std::shared_ptr<Buffer>;
  explicit Buffer();
  ~Buffer();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Buffer)

  ssize_t Size() const;
  void Append(const char* _str);
  void Append(const char* _str, int _size);

  void SetBuf(const char* _str);
  std::string& GetBuf();
  const char* Cstr() const;

  char& operator[](int i);

  void ClearBuffer();
  void GetLine();  // get one line from stdin input

 private:
  std::string buf_;
};

}  // namespace cppserver_core

#endif