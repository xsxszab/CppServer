#include "acceptor.h"

#include <iostream>

#include "channel.h"
#include "event_loop.h"
#include "inet_address.h"
#include "socket_class.h"

namespace cppserver_core {

Acceptor::Acceptor(EventLoop* loop, std::shared_ptr<InetAddress> addr)
    : loop_(loop) {
  sock_ = std::make_unique<Socket>();
  sock_->Bind(addr);
  // sock->setnonblocking();
  sock_->Listen();
  channel_ = std::make_unique<Channel>(loop_, sock_->GetFd());
  channel_->SetReadCallBack(std::bind(&Acceptor::AcceptConnection, this));
  channel_->EnableRead();
}

Acceptor::~Acceptor() {}

void Acceptor::AcceptConnection() const {
  InetAddress* clnt_addr = new InetAddress();
  Socket* clnt_sock = new Socket(sock_->Accept(clnt_addr));
  std::cout << "new client fd: " << clnt_sock->GetFd()
            << " IP: " << inet_ntoa(clnt_addr->GetAddr().sin_addr)
            << " Port: " << ntohs(clnt_addr->GetAddr().sin_port) << std::endl;
  clnt_sock->Setnonblocking();
  new_connection_callback_(clnt_sock->GetFd());
  delete clnt_addr;
}

void Acceptor::SetNewConnectionCallBack(std::function<void(int)> const& func) {
  new_connection_callback_ = func;
}

}  // namespace cppserver_core
