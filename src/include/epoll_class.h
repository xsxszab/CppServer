#ifndef EPOLL_CLASS_H
#define EPOLL_CLASS_H

#include <sys/epoll.h>

#include <vector>

#include "marcos.h"

class Channel;

class Epoll {
 private:
  int epfd;
  struct epoll_event* events;

 public:
  Epoll();
  ~Epoll();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Epoll)

  // void add_fd(int fd, uint32_t op);
  std::vector<Channel*> poll(int timeout = -1);

  void updateChannel(Channel*);
};

#endif