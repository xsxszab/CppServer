#include "custom_exception.h"

namespace cppserver_core {

Exception::Exception(std::string what) : message_(what) {}

const char* Exception::GetMessage() { return message_.c_str(); }

}  // namespace cppserver_core
