#include "connection.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "buffer.h"
#include "channel.h"
#include "socket_class.h"
#include "utilfunc.h"

#define BUFFER_SIZE 1024

namespace cppserver_core {

Connection::Connection(EventLoop* loop, int fd) {
  sock_ = std::make_unique<Socket>();
  sock_->SetFd(fd);
  if (loop != nullptr) {  // bind to a event loop
    channel_ = std::make_unique<Channel>(loop, sock_->GetFd());
    channel_->EnableRead();
    channel_->UseET();
  }

  read_buffer_ = std::make_unique<Buffer>();
  write_buffer_ = std::make_unique<Buffer>();
  state_ = State::Connected;
}

Connection::~Connection() {}

void Connection::Read() {
  if (state_ != State::Connected) {
    std::cout << "connection is disconnected" << std::endl;
    return;
  }
  read_buffer_->ClearBuffer();
  if (sock_->IsNonBlocking()) {
    ReadNonBlocking();
  } else {
    ReadBlocking();
  }
}

void Connection::Write() {
  if (state_ != State::Connected) {
    std::cout << "connection is disconnected" << std::endl;
    return;
  }
  if (sock_->IsNonBlocking()) {
    WriteNonBlocking();
  } else {
    WriteBlocking();
  }
  write_buffer_->ClearBuffer();
}

void Connection::SetDeleteConnectionCallBack(
    std::function<void(int)> const& func) {
  delete_connection_callback_ = func;
}

void Connection::SetOnConnectCallBack(
    std::function<void(Connection*)> const& func) {
  on_connect_callback_ = func;
}

void Connection::SetOnMessageCallBack(
    std::function<void(Connection*)> const& func) {
  on_message_callback_ = func;
  channel_->SetReadCallBack(std::bind(&Connection::Business, this));
}

void Connection::Business() {
  Read();
  on_message_callback_(this);
}

Connection::State Connection::GetState() { return state_; }

void Connection::Close() {
  // std::cout << "close connection, call delete callback function" <<
  // std::endl;
  delete_connection_callback_(sock_->GetFd());
}

void Connection::ReadNonBlocking() {
  // std::cout << "read nonblocking" << std::endl;
  int sockfd = sock_->GetFd();
  char buf[BUFFER_SIZE];
  while (true) {
    memset(buf, 0, sizeof(buf));
    ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      read_buffer_->Append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {
      std::cout << "continue reading" << std::endl;
      continue;
    } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      // std::cout << "finish reading" << std::endl;
      break;
    } else if (bytes_read == 0) {
      // std::cout << "EOF, socket " << sockfd << " disconnected" << std::endl;
      state_ = State::Closed;
      break;
    } else {
      std::cout << "other error happened" << std::endl;
      state_ = State::Closed;
      break;
    }
  }
}

void Connection::WriteNonBlocking() {
  // std::cout << "write nonblocking" << std::endl;
  int sockfd = sock_->GetFd();
  int bytes_to_write = write_buffer_->Size();
  // std::cout << "write buffer size:" << bytes_to_write << std::endl;

  char buf[bytes_to_write];
  memcpy(buf, write_buffer_->Cstr(), bytes_to_write);
  int bytes_left = bytes_to_write;
  while (bytes_left > 0) {
    // std::cout << "bytes left to be sent: " << bytes_left << std::endl;
    ssize_t bytes_write =
        ::write(sockfd, buf + bytes_to_write - bytes_left, bytes_left);
    // std::cout << "bytes wrote: " << bytes_write << std::endl;
    if (bytes_write > 0) {
      bytes_left -= bytes_write;
      continue;
    } else if (bytes_write == -1 && errno == EINTR) {
      std::cout << "continue writing" << std::endl;
      continue;
    } else if (bytes_write == -1 && errno == EAGAIN) {
      break;
    } else if (bytes_write == -1) {
      std::cout << "error during writing to socket " << sockfd << std::endl;
      state_ = State::Closed;
      break;
    }
    bytes_left -= bytes_write;
  }
}

void Connection::ReadBlocking() {
  // std::cout << "read blocking" << std::endl;
  int sockfd = sock_->GetFd();
  unsigned int read_size = 0;
  socklen_t len = sizeof(read_size);
  getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &read_size, &len);
  char buf[read_size];
  ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
  if (bytes_read > 0) {
    read_buffer_->Append(buf, bytes_read);
  } else if (bytes_read == 0) {
    // std::cout << "read EOF, fd " << sockfd << " disconnected" << std::endl;
  } else if (bytes_read == -1) {
    std::cout << "error during reading socket " << sockfd << std::endl;
    state_ = State::Closed;
  }
}

void Connection::WriteBlocking() {
  // std::cout << "write blocking" << std::endl;
  int sockfd = sock_->GetFd();
  ssize_t bytes_write =
      ::write(sockfd, write_buffer_->Cstr(), write_buffer_->Size());
  if (bytes_write == -1) {
    std::cout << "error during writing to socket " << sockfd << std::endl;
    state_ = State::Closed;
  }
}

Buffer* Connection::GetReadBuffer() const { return read_buffer_.get(); }

const char* Connection::ReadBuffer() { return read_buffer_->Cstr(); }

Buffer* Connection::GetWriteBuffer() const { return write_buffer_.get(); }

const char* Connection::WriteBuffer() { return write_buffer_->Cstr(); }

void Connection::SetWriteBuffer(const char* str) { write_buffer_->SetBuf(str); }

void Connection::GetLineWriteBuffer() { write_buffer_->GetLine(); }

Socket* Connection::GetSocket() { return sock_.get(); }

}  // namespace cppserver_core
