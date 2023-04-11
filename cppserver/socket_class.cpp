#include "socket_class.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>

#include "inet_address.h"
#include "utilfunc.h"

namespace cppserver_core {

Socket::Socket() {
  fd_ = socket(AF_INET, SOCK_STREAM, 0);
  Errif(fd_ == -1, "socket create error");
}

Socket::Socket(int _fd) : fd_(_fd) { Errif(fd_ == -1, "socket create error"); }

Socket::~Socket() {
  if (fd_ != -1) {
    close(fd_);
    fd_ = -1;
  }
}

void Socket::Bind(std::shared_ptr<InetAddress> _addr) {
  struct sockaddr_in addr = _addr->GetAddr();
  socklen_t addr_len = _addr->GetAddrLen();
  Errif(::bind(fd_, (sockaddr*)&addr, addr_len) == -1, "socket bind error");
  _addr->SetAddr(addr, addr_len);
}

void Socket::Bind(const char* ip, uint16_t port) {
  auto addr = std::make_shared<InetAddress>(ip, port);
  Bind(addr);
}

void Socket::Listen() {
  Errif(::listen(fd_, SOMAXCONN) == -1, "socket listen error");
}

void Socket::Setnonblocking() {
  ::fcntl(fd_, F_SETFL, fcntl(fd_, F_GETFL) | O_NONBLOCK);
}

void Socket::Connect(InetAddress* addr) {
  struct sockaddr_in temp_addr = addr->GetAddr();
  if (fcntl(fd_, F_GETFL) & O_NONBLOCK) {
    while (true) {
      int res = ::connect(fd_, (sockaddr*)&temp_addr, sizeof(temp_addr));
      if (res == 0) {
        break;
      } else if (res == -1 && errno == EINPROGRESS) {
        continue;
      } else if (res == -1) {
        Errif(true, "socket connect error");
      }
    }
  } else {
    Errif(::connect(fd_, (sockaddr*)&temp_addr, sizeof(temp_addr)) == -1,
          "socket connect error");
  }
}

void Socket::Connect(const char* ip, uint16_t port) {
  InetAddress* addr = new InetAddress(ip, port);
  Connect(addr);
  delete addr;
}

int Socket::Accept(InetAddress* _addr) {
  struct sockaddr_in addr {};
  socklen_t addr_len = sizeof(addr);
  memset(&addr, '\0', sizeof(addr));
  int clnt_fd;
  if (fcntl(fd_, F_GETFL) & O_NONBLOCK) {
    while (true) {
      clnt_fd = ::accept(fd_, (sockaddr*)&addr, &addr_len);
      if (clnt_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
        continue;
      } else if (clnt_fd == -1) {
        Errif(true, "socket accept error");
      } else
        break;
    }
  } else {
    clnt_fd = ::accept(fd_, (sockaddr*)&addr, &addr_len);
    Errif(clnt_fd == -1, "socket accept error");
  }

  _addr->SetAddr(addr, addr_len);
  return clnt_fd;
}

bool Socket::IsNonBlocking() { return (fcntl(fd_, F_GETFL) & O_NONBLOCK) != 0; }

void Socket::SetFd(int _fd) { fd_ = _fd; }

int Socket::GetFd() { return fd_; }

}  // namespace cppserver_core
