#ifndef SOCKET_CLASS_H
#define SOCKET_CLASS_H

#include <arpa/inet.h>

#include "marcos.h"

namespace cppserver_core {

class InetAddress;

class Socket {
 private:
  int fd;

 public:
  Socket();
  Socket(int _fd);
  ~Socket();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Socket)

  void bind(InetAddress*);
  void listen();
  void setnonblocking();

  void connect(InetAddress* addr);
  void connect(const char* ip, uint16_t port);
  int accept(InetAddress*);

  bool isNonBlocking();

  int getFd();
};

}

#endif