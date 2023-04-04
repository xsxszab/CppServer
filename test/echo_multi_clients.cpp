#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#include "cppserver.h"

// void client_func(int num_msgs, int sleep_seconds, int thread_id) {
// std::cout << "thread " << thread_id << ": start thread" << std::endl;
// Socket* sock = new Socket();
// InetAddress* addr = new InetAddress("127.0.0.1", 8888);
// sock->connect(addr);
// Buffer* read_buffer = new Buffer();
// Buffer* write_buffer = new Buffer();

// sleep(sleep_seconds);
// int fd = sock->getFd();
// for (int i = 0; i < num_msgs; i++) {
//   write_buffer->setBuf("msg from client");
//   ssize_t bytes_write =
//       write(fd, write_buffer->c_str(), write_buffer->size());
//   if (bytes_write == -1) {
//     std::cout << "thread " << thread_id
//               << ": socket already disconnected, cannot write" << std::endl;
//     break;
//   }

//   int total_bytes_read = 0;
//   char buf[256];
//   while (true) {
//     memset(&buf, 0, sizeof(buf));
//     ssize_t bytes_read = read(fd, buf, sizeof(buf));
//     if (bytes_read > 0) {
//       read_buffer->append(buf, bytes_read);
//       total_bytes_read += bytes_read;
//     } else if (bytes_read == 0) {
//       std::cout << "thread " << thread_id << ": server disconnected"
//                 << std::endl;
//       exit(EXIT_SUCCESS);
//     }

//     if (total_bytes_read >= write_buffer->size()) {
//       std::cout << "thread " << thread_id
//                 << ": message from server: " << read_buffer->c_str()
//                 << std::endl;
//       break;
//     }
//     read_buffer->clearBuffer();
//   }
// }
// delete sock;
// delete addr;
// delete read_buffer;
// delete write_buffer;
// }

void client_func(int num_msgs, int sleep_seconds, int thread_id) {
  sleep(sleep_seconds);

  Socket* sock = new Socket();
  sock->connect("127.0.0.1", 8888);
  Connection* conn = new Connection(nullptr, sock);

  for (int i = 0; i < num_msgs; i++) {
    std::string msg = "echo from client fd ";
    msg += std::to_string(thread_id);
    msg += ", count ";
    msg += std::to_string(i + 1);
    conn->setWriteBuffer(msg.c_str());
    conn->write();
    if (conn->getState() == Connection::State::Closed) {
      conn->close();
      break;
    }
    conn->read();
    std::cout << conn->readBuffer() << std::endl;
  }

  // delete sock;
  delete conn;
}

int main() {
  int num_threads = 100;
  int msgs = 100;
  int wait = 0;

  ThreadPool* pool = new ThreadPool(num_threads);
  for (int i = 0; i < num_threads; i++) {
    pool->add(client_func, msgs, wait, i);
  }

  delete pool;
  return 0;
}