#include "channel.h"

#include <unistd.h>

#include "epoll_class.h"
#include "event_loop.h"

namespace cppserver_core {

Channel::Channel(EventLoop* _el, int _fd)
    : loop_(_el),
      fd_(_fd),
      listen_events_(0),
      ready_events_(0),
      in_epoll_(false) {}

Channel::~Channel() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}

int Channel::GetFd() { return fd_; }

uint32_t Channel::GetListenEvents() { return listen_events_; }

uint32_t Channel::GetReadyEvents() { return ready_events_; }

bool Channel::InEpoll() { return in_epoll_; }

void Channel::SetInEpoll(bool _in_epoll) { in_epoll_ = _in_epoll; }

void Channel::UseET() {
  listen_events_ |= EPOLLET;
  loop_->UpdateChannel(this);
}

void Channel::SetReadyEvents(uint32_t ev) { ready_events_ = ev; }

void Channel::SetReadCallBack(std::function<void()> func) {
  read_callback_ = func;
}

void Channel::HandleEvent() {
  if (ready_events_ & (EPOLLIN | EPOLLPRI)) {
    read_callback_();
  }

  if (ready_events_ & EPOLLOUT) {
    write_callback_();
  }
}

void Channel::EnableReading() {
  listen_events_ |= EPOLLIN | EPOLLPRI;
  loop_->UpdateChannel(this);
}

}  // namespace cppserver_core
