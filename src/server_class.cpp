#include "server_class.h"

#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <functional>
#include <iostream>
#include <thread>

#include "acceptor.h"
#include "channel.h"
#include "connection.h"
#include "event_loop.h"
#include "inet_address.h"
#include "socket_class.h"
#include "thread_pool.h"
#include "utilfunc.h"

#define MAX_EVENTS 1024
#define BUFFER_SIZE 1024

Server::Server(EventLoop* _loop) : main_reactor(_loop), acceptor(nullptr) {
  acceptor = new Acceptor(main_reactor);
  acceptor->setNewConnectionCallBack(
      std::bind(&Server::newConnection, this, std::placeholders::_1));

  int num_threads = std::thread::hardware_concurrency();
  std::cout << "thread pool size: " << num_threads << std::endl;
  threadpool = new ThreadPool(num_threads);

  for (int i = 0; i < num_threads; i++) sub_reactors.push_back(new EventLoop());

  for (int i = 0; i < num_threads; i++) {
    std::function<void()> sub_loop =
        std::bind(&EventLoop::loop, sub_reactors[i]);
    threadpool->add(std::move(sub_loop));
  }
}

Server::~Server() {
  for (auto* loop : sub_reactors) delete loop;
  delete acceptor;
  delete threadpool;
}

void Server::newConnection(Socket* sock) {
  errif(sock->getFd() == -1, "invalid socket fd");
  int rand_choice = sock->getFd() % sub_reactors.size();
  Connection* conn = new Connection(sub_reactors[rand_choice], sock);
  // std::cout << "init new connection, register callback functions" <<
  // std::endl;
  conn->setDeleteConnectionCallBack(
      std::bind(&Server::deleteConnection, this, std::placeholders::_1));
  // conn->setOnConnectCallBack(on_connection_callback);
  conn->setOnMessageCallBack(on_message_callback);
  connections[sock->getFd()] = conn;
}

void Server::deleteConnection(Socket* sock) {
  std::cout << "delete registered connection instance" << std::endl;
  int fd = sock->getFd();
  if (fd != -1) {
    auto it = connections.find(fd);
    if (it != connections.end()) {
      Connection* conn = connections[fd];
      connections.erase(fd);
      delete conn;
      conn = nullptr;
    }
  }
}

void Server::onConnect(std::function<void(Connection*)> func) {
  on_connection_callback = std::move(func);
}

void Server::onMessage(std::function<void(Connection*)> func) {
  on_message_callback = std::move(func);
}

void Server::newConnect(std::function<void(Connection*)> func) {
  on_message_callback = std::move(func);
}
