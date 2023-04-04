#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>

#include "marcos.h"

namespace cppserver_core {

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

  void Read();
  void Write();

  void SetDeleteConnectionCallBack(std::function<void(Socket*)> const& func);
  void SetOnConnectCallBack(std::function<void(Connection*)> const& func);
  void SetOnMessageCallBack(std::function<void(Connection*)> const& func);
  void Business();

  State GetState();
  void Close();

  Buffer* GetReadBuffer();
  const char* ReadBuffer();
  Buffer* GetWriteBuffer();
  const char* WriteBuffer();
  void SetWriteBuffer(const char* str);
  void GetLineWriteBuffer();

  Socket* GetSocket();

 private:
  EventLoop* loop_;
  Buffer* read_buffer_;
  Buffer* write_buffer_;
  Socket* sock_;
  Channel* channel_;
  std::function<void(Socket*)> delete_connection_callback_;
  std::function<void(Connection*)> on_connect_callback_;
  std::function<void(Connection*)> on_message_callback_;
  State state_{State::Invalid};

  void ReadNonBlocking();
  void WriteNonBlocking();
  void ReadBlocking();
  void WriteBlocking();
};

}  // namespace cppserver_core

#endif