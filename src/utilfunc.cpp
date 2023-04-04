#include <fcntl.h>

#include <cstdio>
#include <cstdlib>

namespace cppserver_core {

void errif(bool condition, const char* errmsg) {
  if (condition) {
    perror(errmsg);
    exit(EXIT_FAILURE);
  }
}

}  // namespace cppserver_core
