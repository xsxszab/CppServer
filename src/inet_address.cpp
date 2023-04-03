#include <cstring>

#include "inet_address.h"

InetAddress::InetAddress() : addr_len(sizeof(addr)) {
  memset(&addr, '\0', sizeof(addr));
}

InetAddress::InetAddress(const char *ip, uint16_t port) {
  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);
  addr_len = sizeof(addr);
}

InetAddress::~InetAddress() {}

void InetAddress::setAddr(sockaddr_in _addr, socklen_t _addr_len) {
  addr = _addr;
  addr_len = _addr_len;
}

sockaddr_in InetAddress::getAddr() { return addr; }

socklen_t InetAddress::getAddrLen() { return addr_len; }
