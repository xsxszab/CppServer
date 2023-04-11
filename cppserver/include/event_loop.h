#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <functional>

#include "marcos.h"

namespace cppserver_core {

class Epoll;
class Channel;

class EventLoop {
 private:
  Epoll* ep_{nullptr};
  bool quit_{false};

 public:
  EventLoop();
  ~EventLoop();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(EventLoop)

  void Loop();
  void UpdateChannel(Channel* channel);
};

}  // namespace cppserver_core

#endif