#include <fcntl.h>

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

namespace cppserver_core {

void Errif(bool condition, const char* errmsg) {
  if (condition) {
    throw std::runtime_error(errmsg);
  }
}

}  // namespace cppserver_core
