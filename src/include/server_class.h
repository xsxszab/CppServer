#ifndef SERVER_CLASS_H
#define SERVER_CLASS_H

#include <unordered_map>
#include <vector>

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

  std::vector<EventLoop*> sub_reactor;
  std::unordered_map<int, Connection*> connections;

 public:
  Server(EventLoop*);
  ~Server();

  void newConnection(Socket*);
  void deleteConnection(Socket*);
};

#endif
