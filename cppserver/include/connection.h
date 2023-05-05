#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>
#include <memory>

#include "marcos.h"

namespace cppserver_core {

class EventLoop;
class Socket;
class Channel;
class Buffer;

// class respobsible for handling TCP connections.
// one Connection instance corresponds to one TCP connection.
class Connection {
 public:
  using ptr = std::shared_ptr<Connection>;

  enum State {
    Invalid = 1,
    HandShaking = 2,
    Connected = 3,
    Closed = 4,
    Failed = 5,
  };

  Connection(EventLoop* _loop, int fd);
  ~Connection();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(Connection)

  void Read();
  void Write();

  void SetDeleteConnectionCallBack(std::function<void(int)> const& func);
  void SetOnConnectCallBack(std::function<void(Connection*)> const& func);
  void SetOnMessageCallBack(std::function<void(Connection*)> const& func);
  void BusinessLogic();

  State GetState();
  void Close();  // close managed TCP connection

  Buffer* GetReadBuffer() const;
  const char* ReadBuffer();
  Buffer* GetWriteBuffer() const;
  const char* WriteBuffer();
  void SetWriteBuffer(const char* str);
  void GetLineWriteBuffer();

  Socket* GetSocket();

 private:
  std::unique_ptr<Buffer> read_buffer_;
  std::unique_ptr<Buffer> write_buffer_;
  std::unique_ptr<Socket> sock_;
  std::unique_ptr<Channel> channel_;
  std::function<void(int)> delete_connection_callback_;
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