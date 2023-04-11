#ifndef EPOLL_CLASS_H
#define EPOLL_CLASS_H

#include <sys/epoll.h>

#include <vector>

#include "marcos.h"

namespace cppserver_core {

class Channel;

class Epoll {
 public:
  Epoll();
  ~Epoll();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Epoll)

  // void add_fd(int fd, uint32_t op);
  std::vector<Channel*> Poll(int timeout = -1);

  void UpdateChannel(Channel* channel);

 private:
  int epfd_{-1};
  struct epoll_event* events_{nullptr};
};

}  // namespace cppserver_core

#endif