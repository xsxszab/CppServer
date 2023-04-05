#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

#include "marcos.h"

namespace cppserver_core {

class ThreadPool {
 private:
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> task_queue_;
  std::mutex tasks_mtx_;
  std::condition_variable cv_;

  bool stop_;

 public:
  explicit ThreadPool(int num_thread = 8);
  ~ThreadPool();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(ThreadPool)

  template <class F, class... Args>
  auto Add(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>;
};

// perfect forwarding
template <class F, class... Args>
inline auto ThreadPool::Add(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;
  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(tasks_mtx_);
    if (stop_) {
      throw std::runtime_error("thread pool already stopped\n");
    }

    task_queue_.emplace([task]() { (*task)(); });
  }
  cv_.notify_one();
  return res;
}

}  // namespace cppserver_core

#endif