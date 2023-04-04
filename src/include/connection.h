#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>

#include "marcos.h"

class EventLoop;
class Socket;
class Channel;
class Buffer;

class Connection {
 public:
  enum State {
    Invalid = 1,
    HandShaking = 2,
    Connected = 3,
    Closed = 4,
    Failed = 5,
  };

  Connection(EventLoop* _loop, Socket* _sock);
  ~Connection();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Connection)

  void read();
  void write();

  void setDeleteConnectionCallBack(std::function<void(Socket*)> const&);
  void setOnConnectCallBack(std::function<void(Connection*)> const&);
  void setOnMessageCallBack(std::function<void(Connection*)> const&);
  void business();

  State getState();
  void close();

  Buffer* getReadBuffer();
  const char* readBuffer();
  Buffer* getWriteBuffer();
  const char* writeBuffer();
  void setWriteBuffer(const char* str);
  void getLineWriteBuffer();

  Socket* getSocket();

 private:
  EventLoop* loop;
  Buffer* read_buffer;
  Buffer* write_buffer;
  Socket* sock;
  Channel* channel;
  std::function<void(Socket*)> delete_connection_callback;
  std::function<void(Connection*)> on_connect_callback;
  std::function<void(Connection*)> on_message_callback;
  State state{State::Invalid};

  void readNonBlocking();
  void writeNonBlocking();
  void readBlocking();
  void writeBlocking();
};

#endif