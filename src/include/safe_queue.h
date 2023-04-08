#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <memory>
#include <mutex>
#include <queue>

#include "utilfunc.h"

namespace cppserver_core {

// thread save queue
template <class T>
class SafeQueue {
 public:
  using guard = std::lock_guard<std::mutex>;
  using unique_lock = std::unique_lock<std::mutex>;
  SafeQueue(int max_size) : max_size_(max_size) {}
  ~SafeQueue() = default;

  void Clear() {}

  bool IsEmpty() const {
    guard lock(mtx_);
    return queue_.size();
  }

  T& Front() const {
    guard lock(mtx_);
    return queue_.front();
  }

  T& Back() const {
    guard lock(mtx_);
    return queue_.back();
  }

  void Push(T& t) {
    guard lock(mtx_);
    queue_.emplace(t);
  }

  bool Pop(T& t) {
    guard lock(mtx_);
    if (queue_.empty()) {
      return false;
    }
    t = std::move(queue_.front());
    queue_.pop();
    return true;
  }

 private:
  int max_size_{0};
  std::queue<T> queue_;
  std::mutex mtx_;
};

}  // namespace cppserver_core

#endif