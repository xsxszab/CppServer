#include <arpa/inet.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <thread>
#include <unistd.h>

#include "acceptor.h"
#include "channel.h"
#include "connection.h"
#include "event_loop.h"
#include "inet_address.h"
#include "server_class.h"
#include "socket_class.h"
#include "thread_pool.h"

#define MAX_EVENTS 1024
#define BUFFER_SIZE 1024

Server::Server(EventLoop *_loop) : main_reactor(_loop), acceptor(nullptr) {
  acceptor = new Acceptor(main_reactor);
  acceptor->setNewConnectionCallBack(
      std::bind(&Server::newConnection, this, std::placeholders::_1));

  int num_threads = std::thread::hardware_concurrency();
  threadpool = new ThreadPool(num_threads);

  for (int i = 0; i < num_threads; i++)
    sub_reactor.push_back(new EventLoop());

  for (int i = 0; i < num_threads; i++) {
    std::function<void()> sub_loop =
        std::bind(&EventLoop::loop, sub_reactor[i]);
    threadpool->add(sub_loop);
  }
}

Server::~Server() {
  delete acceptor;
  delete threadpool;
}

void Server::newConnection(Socket *sock) {
  if (sock->getFd() != -1) {
    int rand_choice = sock->getFd() % sub_reactor.size();
    Connection *conn = new Connection(sub_reactor[rand_choice], sock);
    conn->setDeleteConnectionCallBack(
        std::bind(&Server::deleteConnection, this, std::placeholders::_1));
    connections[sock->getFd()] = conn;
  }
}

void Server::deleteConnection(Socket *sock) {
  int fd = sock->getFd();
  if (fd != -1) {
    auto it = connections.find(fd);
    if (it != connections.end()) {
      Connection *conn = connections[fd];
      connections.erase(fd);
      delete conn;
    }
  }
}
