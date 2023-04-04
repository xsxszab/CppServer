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
  EventLoop* main_reactor;
  Acceptor* acceptor;
  ThreadPool* threadpool;

  std::vector<EventLoop*> sub_reactors;
  std::unordered_map<int, Connection*> connections;

  std::function<void(Connection*)> on_connection_callback;
  std::function<void(Connection*)> new_connection_callback;
  std::function<void(Connection*)> on_message_callback;

 public:
  explicit Server(EventLoop*);
  ~Server();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Server)

  void newConnection(Socket*);
  void deleteConnection(Socket*);
  void onConnect(std::function<void(Connection*)>);
  void onMessage(std::function<void(Connection*)>);
  void newConnect(std::function<void(Connection*)>);
};

}  // namespace cppserver_core

#endif
