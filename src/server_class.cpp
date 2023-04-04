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

namespace cppserver_core {

Server::Server(EventLoop* _loop) : main_reactor_(_loop), acceptor_(nullptr) {
  acceptor_ = new Acceptor(main_reactor_);
  acceptor_->SetNewConnectionCallBack(
      std::bind(&Server::NewConnection, this, std::placeholders::_1));

  int num_threads = std::thread::hardware_concurrency();
  std::cout << "thread pool size: " << num_threads << std::endl;
  threadpool_ = new ThreadPool(num_threads);

  for (int i = 0; i < num_threads; i++) {
    sub_reactors_.push_back(new EventLoop());
  }

  for (int i = 0; i < num_threads; i++) {
    std::function<void()> sub_loop =
        std::bind(&EventLoop::Loop, sub_reactors_[i]);
    threadpool_->Add(std::move(sub_loop));
  }
}

Server::~Server() {
  for (auto* loop : sub_reactors_) {
    delete loop;
  }
  delete acceptor_;
  delete threadpool_;
}

void Server::NewConnection(Socket* sock) {
  Errif(sock->GetFd() == -1, "invalid socket fd");
  int rand_choice = sock->GetFd() % sub_reactors_.size();
  Connection* conn = new Connection(sub_reactors_[rand_choice], sock);
  // std::cout << "init new connection, register callback functions" <<
  // std::endl;
  conn->SetDeleteConnectionCallBack(
      std::bind(&Server::DeleteConnection, this, std::placeholders::_1));
  // conn->setOnConnectCallBack(on_connection_callback);
  conn->SetOnMessageCallBack(on_message_callback_);
  connections_[sock->GetFd()] = conn;
}

void Server::DeleteConnection(Socket* sock) {
  std::cout << "delete registered connection instance" << std::endl;
  int fd = sock->GetFd();
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