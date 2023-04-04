#include "epoll_class.h"

#include <unistd.h>

#include <cstring>
#include <iostream>

#include "channel.h"
#include "utilfunc.h"

#define MAX_EVENTS 1000

namespace cppserver_core {

Epoll::Epoll() {
  epfd_ = epoll_create1(0);
  Errif(epfd_ == -1, "epoll create error");

  events_ = new epoll_event[MAX_EVENTS];
  memset(events_, '\0', sizeof(*events_) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (epfd_ != -1) {
    close(epfd_);
    epfd_ = -1;
  }
  delete[] events_;
}

// void Epoll::add_fd(int fd, uint32_t op)
// {
//     struct epoll_event ev;
//     memset(&ev, '\0', sizeof(ev));

//     ev.data.fd = fd;
//     ev.events = op;
//     errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add event
//     error");
// }

std::vector<Channel*> Epoll::Poll(int timeout) {
  std::vector<Channel*> active_channels;
  int nfds = epoll_wait(epfd_, events_, MAX_EVENTS, timeout);
  Errif(nfds == -1, "epoll wait error");
  for (int i = 0; i < nfds; i++) {
    Channel* ch = (Channel*)events_[i].data.ptr;
    ch->SetReadyEvents(events_[i].events);
    active_channels.emplace_back(ch);
  }
  return active_channels;
}

void Epoll::UpdateChannel(Channel* channel) {
  int fd = channel->GetFd();
  struct epoll_event ev {};
  memset(&ev, '\0', sizeof(ev));
  ev.data.ptr = channel;
  ev.events = channel->GetListenEvents();
  if (!channel->InEpoll()) {
    Errif(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
    channel->SetInEpoll();
  } else {
    Errif(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
  }
}

}  // namespace cppserver_core
