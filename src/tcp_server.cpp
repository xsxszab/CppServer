#include "tcp_server.h"

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

namespace cppserver_core {

Server::Server() : acceptor_(nullptr) {
  main_reactor_ = std::make_unique<EventLoop>();
  acceptor_ = std::make_unique<Acceptor>(main_reactor_.get());
  acceptor_->SetNewConnectionCallBack(
      std::bind(&Server::NewConnection, this, std::placeholders::_1));

  int num_threads = std::thread::hardware_concurrency();
  std::cout << "thread pool size: " << num_threads << std::endl;
  threadpool_ = std::make_unique<ThreadPool>(num_threads);

  for (int i = 0; i < num_threads; i++) {
    sub_reactors_.push_back(new EventLoop());
  }
}

Server::~Server() {
  for (auto* loop : sub_reactors_) {
    delete loop;
  }
}

void Server::Start() {
  for (unsigned int i = 0; i < sub_reactors_.size(); i++) {
    std::function<void()> sub_loop =
        std::bind(&EventLoop::Loop, sub_reactors_[i]);
    threadpool_->Add(std::move(sub_loop));
  }

  main_reactor_->Loop();
}

void Server::NewConnection(int fd) {
  Errif(fd == -1, "invalid socket fd");
  int rand_choice = fd % sub_reactors_.size();
  Connection* conn = new Connection(sub_reactors_[rand_choice], fd);
  // std::cout << "init new connection, register callback functions" <<
  // std::endl;
  conn->SetDeleteConnectionCallBack(
      std::bind(&Server::DeleteConnection, this, std::placeholders::_1));
  // conn->setOnConnectCallBack(on_connection_callback);
  conn->SetOnMessageCallBack(on_message_callback_);
  connections_[fd] = conn;
}

void Server::DeleteConnection(int fd) {
  std::cout << "delete registered connection instance" << std::endl;
  if (fd != -1) {
    auto it = connections_.find(fd);
    if (it != connections_.end()) {
      Connection* conn = connections_[fd];
      connections_.erase(fd);
      delete conn;
      conn = nullptr;
    }
  }
}

void Server::OnConnect(std::function<void(Connection*)> func) {
  on_connection_callback_ = std::move(func);
}

void Server::OnMessage(std::function<void(Connection*)> func) {
  on_message_callback_ = std::move(func);
}

void Server::NewConnect(std::function<void(Connection*)> func) {
  on_message_callback_ = std::move(func);
}

}  // namespace cppserver_core