#include <cstring>
#include <iostream>
#include <unistd.h>

#include "channel.h"
#include "epoll_class.h"
#include "utilfunc.h"

#define MAX_EVENTS 1000

Epoll::Epoll() : epfd(-1), events(nullptr) {
  epfd = epoll_create1(0);
  errif(epfd == -1, "epoll create error");

  events = new epoll_event[MAX_EVENTS];
  memset(events, '\0', sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll() {
  if (epfd != -1) {
    close(epfd);
    epfd = -1;
  }
  delete[] events;
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

std::vector<Channel *> Epoll::poll(int timeout) {
  std::vector<Channel *> active_channels;
  int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
  errif(nfds == -1, "epoll wait error");
  for (int i = 0; i < nfds; i++) {
    Channel *ch = (Channel *)events[i].data.ptr;
    ch->setReady(events[i].events);
    active_channels.emplace_back(ch);
  }
  return active_channels;
}

void Epoll::updateChannel(Channel *channel) {
  int fd = channel->getFd();
  struct epoll_event ev;
  memset(&ev, '\0', sizeof(ev));
  ev.data.ptr = channel;
  ev.events = channel->getEvents();
  if (!channel->inEpoll()) {
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
    channel->setInEpoll();
  } else {
    errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
  }
}
