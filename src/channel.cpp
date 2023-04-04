#include "channel.h"

#include <unistd.h>

#include "epoll_class.h"
#include "event_loop.h"

Channel::Channel(EventLoop* _el, int _fd)
    : loop(_el), fd(_fd), listen_events(0), ready_events(0), in_epoll(false) {}

Channel::~Channel() {
  if (fd != -1) {
    close(fd);
    fd = -1;
  }
}

int Channel::getFd() { return fd; }

uint32_t Channel::getListenEvents() { return listen_events; }

uint32_t Channel::getReadyEvents() { return ready_events; }

bool Channel::inEpoll() { return in_epoll; }

void Channel::setInEpoll(bool _in_epoll) { in_epoll = _in_epoll; }

void Channel::useET() {
  listen_events |= EPOLLET;
  loop->updateChannel(this);
}

void Channel::setReadyEvents(uint32_t _ev) { ready_events = _ev; }

void Channel::setReadCallBack(std::function<void()> func) {
  read_callback = func;
}

void Channel::handleEvent() {
  if (ready_events & (EPOLLIN | EPOLLPRI)) {
    read_callback();
  }

  if (ready_events & EPOLLOUT) {
    write_callback();
  }
}

void Channel::enableReading() {
  listen_events |= EPOLLIN | EPOLLPRI;
  loop->updateChannel(this);
}
