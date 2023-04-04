#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>

#include <functional>

#include "marcos.h"

namespace cppserver_core {

class Epoll;
class EventLoop;

class Channel {
 private:
  EventLoop* loop_;
  int fd_;
  uint32_t listen_events_;
  uint32_t ready_events_;

  bool in_epoll_;

  std::function<void()> read_callback_;
  std::function<void()> write_callback_;

 public:
  Channel(EventLoop* _el, int _fd);
  ~Channel();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Channel)

  void EnableReading();

  int GetFd();
  uint32_t GetListenEvents();
  uint32_t GetReadyEvents();
  bool InEpoll();
  void SetInEpoll(bool _in_epoll = true);

  void UseET();

  void SetReadyEvents(uint32_t ev);

  void SetReadCallBack(std::function<void()> func);
  void HandleEvent();
};

}  // namespace cppserver_core

#endif