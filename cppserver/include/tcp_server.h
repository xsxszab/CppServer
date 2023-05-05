#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "marcos.h"

namespace cppserver_core {

class InetAddress;
class EventLoop;
class Socket;
class Acceptor;
class ThreadPool;
class Connection;

class Server {
 public:
  explicit Server(const char* ip = "127.0.0.1", uint16_t port = 8888);
  ~Server();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Server)

  // push sub reactors into threadpool, start main reactor loop
  void Start();

  void NewConnection(int fd);
  void DeleteConnection(int fd);

  void SetOnConnectCallback(std::function<void(Connection*)> func);
  void SetOnMessageCallback(std::function<void(Connection*)> func);
  void SetNewConnectCallback(std::function<void(Connection*)> func);

 private:
  std::unique_ptr<EventLoop> main_reactor_;  // master reactor
  std::unique_ptr<Acceptor> acceptor_;
  std::unique_ptr<ThreadPool> threadpool_;

  std::vector<EventLoop*> sub_reactors_;  // slave reactors
  std::unordered_map<int, std::unique_ptr<Connection>> connections_;

  std::function<void(Connection*)> on_connection_callback_;
  std::function<void(Connection*)> new_connection_callback_;
  std::function<void(Connection*)> on_message_callback_;

  std::shared_ptr<InetAddress> address_;
};

}  // namespace cppserver_core

#endif
