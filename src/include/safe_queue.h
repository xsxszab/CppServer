#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

namespace cppserver_core {

#include <memory>
#include <mutex>
#include <queue>

#include "utilfunc.h"

// thread save queue
template <class T>
class BlockQueue {
 public:
  using guard = std::lock_guard<std::mutex>;
  using unique_lock = std::unique_lock<std::mutex>;
  BlockQueue() = default;
  ~BlockQueue() = default;

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
  std::queue<T> queue_;
  std::mutex mtx_;
};

}  // namespace cppserver_core

#endif