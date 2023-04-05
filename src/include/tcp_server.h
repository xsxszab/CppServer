#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "marcos.h"

namespace cppserver_core {

class EventLoop;
class Socket;
class Acceptor;
class ThreadPool;
class Connection;

class Server {
 private:
  std::unique_ptr<EventLoop> main_reactor_;
  std::unique_ptr<Acceptor> acceptor_;
  std::unique_ptr<ThreadPool> threadpool_;

  std::vector<EventLoop*> sub_reactors_;
  std::unordered_map<int, Connection*> connections_;

  std::function<void(Connection*)> on_connection_callback_;
  std::function<void(Connection*)> new_connection_callback_;
  std::function<void(Connection*)> on_message_callback_;

 public:
  explicit Server();
  ~Server();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Server)

  void Start();

  void NewConnection(int fd);
  void DeleteConnection(int fd);
  void OnConnect(std::function<void(Connection*)> func);
  void OnMessage(std::function<void(Connection*)> func);
  void NewConnect(std::function<void(Connection*)> func);
};

}  // namespace cppserver_core

#endif
