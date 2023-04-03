#include <iostream>

#include "acceptor.h"
#include "channel.h"
#include "event_loop.h"
#include "inet_address.h"
#include "socket_class.h"

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop) {
  sock = new Socket();
  auto *addr = new InetAddress("127.0.0.1", 8888);
  sock->bind(addr);
  // sock->setnonblocking();
  sock->listen();

  channel = new Channel(loop, sock->getFd());
  channel->setReadCallBack(std::bind(&Acceptor::acceptConnection, this));
  channel->enableReading();
  delete addr;
}

Acceptor::~Acceptor() {
  delete sock;
  delete channel;
}

void Acceptor::acceptConnection() {
  InetAddress *clnt_addr = new InetAddress();
  Socket *clnt_sock = new Socket(sock->accept(clnt_addr));
  std::cout << "new client fd: " << clnt_sock->getFd()
            << " IP: " << inet_ntoa(clnt_addr->getAddr().sin_addr)
            << " Port: " << ntohs(clnt_addr->getAddr().sin_port) << std::endl;
  clnt_sock->setnonblocking();
  newConnectionCallBack(clnt_sock);
  delete clnt_addr;
}

void Acceptor::setNewConnectionCallBack(std::function<void(Socket *)> func) {
  newConnectionCallBack = func;
}
