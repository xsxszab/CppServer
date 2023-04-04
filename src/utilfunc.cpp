#include <fcntl.h>

#include <cstdio>
#include <cstdlib>

namespace cppserver_core {

void Errif(bool condition, const char* errmsg) {
  if (condition) {
    perror(errmsg);
    exit(EXIT_FAILURE);
  }
}

}  // namespace cppserver_core
