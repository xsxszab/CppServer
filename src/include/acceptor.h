#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>

#include "marcos.h"

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
 private:
  EventLoop* loop;
  Socket* sock;
  Channel* channel;

  std::function<void(Socket*)> newConnectionCallBack;

 public:
  explicit Acceptor(EventLoop* _loop);
  ~Acceptor();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Acceptor)

  void acceptConnection();

  void setNewConnectionCallBack(std::function<void(Socket*)>);
};

#endif
