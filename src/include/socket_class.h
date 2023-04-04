#ifndef SOCKET_CLASS_H
#define SOCKET_CLASS_H

#include <arpa/inet.h>

#include "marcos.h"

namespace cppserver_core {

class InetAddress;

class Socket {
 private:
  int fd_{-1};

 public:
  Socket();
  explicit Socket(int _fd);
  ~Socket();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Socket)

  void Bind(InetAddress* _addr);
  void Listen();
  void Setnonblocking();

  void Connect(InetAddress* addr);
  void Connect(const char* ip, uint16_t port);
  int Accept(InetAddress* _addr);

  bool IsNonBlocking();

  int GetFd();
};

}  // namespace cppserver_core

#endif