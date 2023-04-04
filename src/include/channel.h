#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>

#include <functional>

#include "marcos.h"

class Epoll;
class EventLoop;

class Channel {
 private:
  EventLoop* loop;
  int fd;
  uint32_t listen_events;
  uint32_t ready_events;

  bool in_epoll;

  std::function<void()> read_callback;
  std::function<void()> write_callback;

 public:
  Channel(EventLoop* _el, int _fd);
  ~Channel();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Channel)

  void enableReading();

  int getFd();
  uint32_t getListenEvents();
  uint32_t getReadyEvents();
  bool inEpoll();
  void setInEpoll(bool _in_epoll = true);

  void useET();

  void setReadyEvents(uint32_t);

  void setReadCallBack(std::function<void()>);
  void handleEvent();
};

#endif