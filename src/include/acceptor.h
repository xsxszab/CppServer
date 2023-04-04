#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>

#include "marcos.h"

namespace cppserver_core {

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor {
 private:
  EventLoop* loop_;
  Socket* sock_;
  Channel* channel_;

  std::function<void(Socket*)> new_connection_callback_;

 public:
  explicit Acceptor(EventLoop* loop);
  ~Acceptor();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Acceptor)

  void AcceptConnection();

  void SetNewConnectionCallBack(std::function<void(Socket*)> func);
};

}  // namespace cppserver_core

#endif
