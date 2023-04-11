#ifndef SOCKET_CLASS_H
#define SOCKET_CLASS_H

#include <arpa/inet.h>

#include <memory>

#include "marcos.h"

namespace cppserver_core {

class InetAddress;

// wrapper class for linux socket
class Socket {
 private:
  int fd_{-1};

 public:
  Socket();
  explicit Socket(int _fd);
  ~Socket();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Socket)

  void Bind(std::shared_ptr<InetAddress> _addr);
  void Bind(const char* ip, uint16_t port);
  void Listen();
  void Setnonblocking();  // set O_NONBLOCK flag

  void Connect(InetAddress* addr);
  void Connect(const char* ip, uint16_t port);
  int Accept(InetAddress* _addr);

  bool IsNonBlocking();

  void SetFd(int _fd);
  int GetFd();
};

}  // namespace cppserver_core

#endif