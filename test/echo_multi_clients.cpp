#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>

#include "cppserver.h"

using namespace cppserver_core;

void ClientFunc(int num_msgs, int sleep_seconds, int thread_id) {
  sleep(sleep_seconds);

  Socket* sock = new Socket();
  sock->Connect("127.0.0.1", 8888);
  Connection* conn = new Connection(nullptr, sock);

  for (int i = 0; i < num_msgs; i++) {
    std::string msg = "echo from client fd ";
    msg += std::to_string(thread_id);
    msg += ", count ";
    msg += std::to_string(i + 1);
    conn->SetWriteBuffer(msg.c_str());
    conn->Write();
    if (conn->GetState() == Connection::State::Closed) {
      conn->Close();
      break;
    }
    conn->Read();
    std::cout << conn->ReadBuffer() << std::endl;
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
    pool->Add(ClientFunc, msgs, wait, i);
  }

  delete pool;
  return 0;
}