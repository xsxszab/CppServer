#ifndef CUSTOM_EXCEPTION_H
#define CUSTOM_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace cppserver_core {

class Exception : std::exception {
 public:
  Exception(std::string what);
  ~Exception() noexcept override = default;

  const char* GetMessage();

 private:
  std::string message_;
};

}  // namespace cppserver_core

#endif