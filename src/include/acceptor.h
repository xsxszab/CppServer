#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>
#include <memory>

#include "marcos.h"

namespace cppserver_core {

class EventLoop;
class Socket;
class InetAddress;
class Channel;

// class for handling incoming TCP connections.
class Acceptor {
 public:
  explicit Acceptor(EventLoop* loop);
  ~Acceptor();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Acceptor)

  void AcceptConnection() const;

  void SetNewConnectionCallBack(std::function<void(int)> const& func);

 private:
  EventLoop* loop_;
  std::unique_ptr<Socket> sock_;
  std::unique_ptr<Channel> channel_;

  std::function<void(int)> new_connection_callback_;
};

}  // namespace cppserver_core

#endif
