#include "inet_address.h"

#include <cstring>

namespace cppserver_core {

InetAddress::InetAddress() : addr_len_(sizeof(addr_)) {
  memset(&addr_, '\0', sizeof(addr_));
}

InetAddress::InetAddress(const char* ip, uint16_t port) {
  memset(&addr_, '\0', sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = inet_addr(ip);
  addr_.sin_port = htons(port);
  addr_len_ = sizeof(addr_);
}

InetAddress::~InetAddress() = default;

void InetAddress::SetAddr(sockaddr_in addr, socklen_t addr_len) {
  addr_ = addr;
  addr_len_ = addr_len;
}

sockaddr_in InetAddress::GetAddr() { return addr_; }

socklen_t InetAddress::GetAddrLen() { return addr_len_; }

}  // namespace cppserver_core