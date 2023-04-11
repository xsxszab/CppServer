#include "thread_pool.h"

namespace cppserver_core {

ThreadPool::ThreadPool(int num_thread) : stop_(false) {
  for (int i = 0; i < num_thread; i++) {
    threads_.emplace_back(std::thread([this]() {
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock<std::mutex> lock(tasks_mtx_);
          cv_.wait(lock, [this]() { return stop_ || !task_queue_.empty(); });
          if (stop_ && task_queue_.empty()) {
            return;
          }
          task = task_queue_.front();
          task_queue_.pop();
        }
        task();
      }
    }));
  }
}

ThreadPool::~ThreadPool() {
  {
    std::unique_lock<std::mutex> lock(tasks_mtx_);
    stop_ = true;
  }
  cv_.notify_all();
  for (auto& thread : threads_) {
    if (thread.joinable()) {
      thread.join();
    }
  }
}

}  // namespace cppserver_core
