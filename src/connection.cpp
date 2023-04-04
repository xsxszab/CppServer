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

Connection::Connection(EventLoop* _loop, Socket* _sock)
    : loop(_loop),
      read_buffer(nullptr),
      write_buffer(nullptr),
      sock(_sock),
      channel(nullptr) {
  if (loop != nullptr) {  // bind to a event loop
    channel = new Channel(loop, sock->getFd());
    channel->enableReading();
    channel->useET();
  }

  read_buffer = new Buffer();
  write_buffer = new Buffer();
  state = State::Connected;
}

Connection::~Connection() {
  if (loop != nullptr) {
    delete channel;
  }
  delete sock;
  delete read_buffer;
  delete write_buffer;
}

void Connection::read() {
  if (state != State::Connected) {
    std::cout << "connection is disconnected" << std::endl;
    return;
  }
  read_buffer->clearBuffer();
  if (sock->isNonBlocking()) {
    readNonBlocking();
  } else {
    readBlocking();
  }
}

void Connection::write() {
  if (state != State::Connected) {
    std::cout << "connection is disconnected" << std::endl;
    return;
  }
  if (sock->isNonBlocking()) {
    writeNonBlocking();
  } else {
    writeBlocking();
  }
  write_buffer->clearBuffer();
}

void Connection::setDeleteConnectionCallBack(
    std::function<void(Socket*)> const& func) {
  delete_connection_callback = func;
}

void Connection::setOnConnectCallBack(
    std::function<void(Connection*)> const& func) {
  on_connect_callback = func;
}

void Connection::setOnMessageCallBack(
    std::function<void(Connection*)> const& func) {
  on_message_callback = func;
  channel->setReadCallBack(std::bind(&Connection::business, this));
}

void Connection::business() {
  read();
  on_message_callback(this);
}

Connection::State Connection::getState() { return state; }

void Connection::close() {
  // std::cout << "close connection, call delete callback function" <<
  // std::endl;
  delete_connection_callback(sock);
}

void Connection::readNonBlocking() {
  // std::cout << "read nonblocking" << std::endl;
  int sockfd = sock->getFd();
  char buf[BUFFER_SIZE];
  while (true) {
    memset(buf, 0, sizeof(buf));
    ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
    if (bytes_read > 0) {
      read_buffer->append(buf, bytes_read);
    } else if (bytes_read == -1 && errno == EINTR) {
      std::cout << "continue reading" << std::endl;
      continue;
    } else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
      // std::cout << "finish reading" << std::endl;
      break;
    } else if (bytes_read == 0) {
      // std::cout << "EOF, socket " << sockfd << " disconnected" << std::endl;
      state = State::Closed;
      break;
    } else {
      std::cout << "other error happened" << std::endl;
      state = State::Closed;
      break;
    }
  }
}

void Connection::writeNonBlocking() {
  // std::cout << "write nonblocking" << std::endl;
  int sockfd = sock->getFd();
  int bytes_to_write = write_buffer->size();
  // std::cout << "write buffer size:" << bytes_to_write << std::endl;

  char buf[bytes_to_write];
  memcpy(buf, write_buffer->c_str(), bytes_to_write);
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
      state = State::Closed;
      break;
    }
    bytes_left -= bytes_write;
  }
}

void Connection::readBlocking() {
  // std::cout << "read blocking" << std::endl;
  int sockfd = sock->getFd();
  unsigned int read_size = 0;
  socklen_t len = sizeof(read_size);
  getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &read_size, &len);
  char buf[read_size];
  ssize_t bytes_read = ::read(sockfd, buf, sizeof(buf));
  if (bytes_read > 0) {
    read_buffer->append(buf, bytes_read);
  } else if (bytes_read == 0) {
    // std::cout << "read EOF, fd " << sockfd << " disconnected" << std::endl;
  } else if (bytes_read == -1) {
    std::cout << "error during reading socket " << sockfd << std::endl;
    state = State::Closed;
  }
}

void Connection::writeBlocking() {
  // std::cout << "write blocking" << std::endl;
  int sockfd = sock->getFd();
  ssize_t bytes_write =
      ::write(sockfd, write_buffer->c_str(), write_buffer->size());
  if (bytes_write == -1) {
    std::cout << "error during writing to socket " << sockfd << std::endl;
    state = State::Closed;
  }
}

Buffer* Connection::getReadBuffer() { return read_buffer; }

const char* Connection::readBuffer() { return read_buffer->c_str(); }

Buffer* Connection::getWriteBuffer() { return write_buffer; }

const char* Connection::writeBuffer() { return write_buffer->c_str(); }

void Connection::setWriteBuffer(const char* str) { write_buffer->setBuf(str); }

void Connection::getLineWriteBuffer() { write_buffer->getLine(); }

Socket* Connection::getSocket() { return sock; }
