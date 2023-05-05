#ifndef CUSTOM_EXCEPTION_H
#define CUSTOM_EXCEPTION_H

#include <stdexcept>
#include <string>

class Exception : std::exception {
 public:
  Exception(std::string what);
  ~Exception() noexcept override = default;

  const char* GetMessage();

 private:
  std::string message_;
};

#endif