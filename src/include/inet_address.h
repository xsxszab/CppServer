#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include <arpa/inet.h>

#include "marcos.h"

namespace cppserver_core {

class InetAddress {
 public:
  struct sockaddr_in addr_;
  socklen_t addr_len_;

  InetAddress();
  InetAddress(const char* ip, uint16_t port);
  ~InetAddress();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(InetAddress)

  void SetAddr(sockaddr_in addr, socklen_t addr_len);

  sockaddr_in GetAddr();
  socklen_t GetAddrLen();
};

}  // namespace cppserver_core

#endif