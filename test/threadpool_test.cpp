#include <iostream>

#include "cppserver.h"

void print_variable(int a, float b, const char* c) {
  std::cout << a << " " << b << " " << c << std::endl;
}

void print_message() { std::cout << "test message" << std::endl; }

int main() {
  ThreadPool* pool = new ThreadPool(std::thread::hardware_concurrency());
  pool->add(std::bind(print_variable, 1, 2.3, "45678"));
  pool->add(print_message);

  delete pool;
  return 0;
}
