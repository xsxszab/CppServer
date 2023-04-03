#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <functional>

class Epoll;
class Channel;
class ThreadPool;

class EventLoop {
 private:
  Epoll* ep;
  ThreadPool* threadpool;
  bool quit;

 public:
  EventLoop();
  ~EventLoop();

  void loop();
  void updateChannel(Channel*);
  void addThread(std::function<void()>);
};

#endif