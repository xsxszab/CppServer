#ifndef SERVER_CLASS_H
#define SERVER_CLASS_H

#include <functional>
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
  EventLoop* main_reactor_;
  Acceptor* acceptor_;
  ThreadPool* threadpool_;

  std::vector<EventLoop*> sub_reactors_;
  std::unordered_map<int, Connection*> connections_;

  std::function<void(Connection*)> on_connection_callback_;
  std::function<void(Connection*)> new_connection_callback_;
  std::function<void(Connection*)> on_message_callback_;

 public:
  explicit Server(EventLoop* _loop);
  ~Server();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Server)

  void NewConnection(Socket* sock);
  void DeleteConnection(Socket* sock);
  void OnConnect(std::function<void(Connection*)> func);
  void OnMessage(std::function<void(Connection*)> func);
  void NewConnect(std::function<void(Connection*)> func);
};

}  // namespace cppserver_core

#endif
