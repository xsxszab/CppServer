#include <iostream>

#include "cppserver.h"

namespace core = cppserver_core;

void PrintVariable(int a, float b, const char* c) {
  std::cout << a << " " << b << " " << c << std::endl;
}

void PrintMessage() { std::cout << "test message" << std::endl; }

int main() {
  core::ThreadPool* pool =
      new core::ThreadPool(std::thread::hardware_concurrency());
  pool->Add(std::bind(PrintVariable, 1, 2.3, "45678"));
  pool->Add(PrintMessage);

  delete pool;
  return 0;
}
