#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <functional>

#include "marcos.h"

namespace cppserver_core {

class Epoll;
class Channel;

class EventLoop {
 private:
  Epoll* ep;
  bool quit;

 public:
  EventLoop();
  ~EventLoop();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(EventLoop)

  void loop();
  void updateChannel(Channel*);
};

}  // namespace cppserver_core

#endif