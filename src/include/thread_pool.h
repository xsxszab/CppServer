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
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> task_queue;
  std::mutex tasks_mtx;
  std::condition_variable cv;

  bool stop;

 public:
  ThreadPool(int num_thread = 8);
  ~ThreadPool();

  DISABLE_COPY_AND_MOVE_CONSTRUCT(ThreadPool)

  template <class F, class... Args>
  auto add(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>;
};

template <class F, class... Args>
inline auto ThreadPool::add(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {
  using return_type = typename std::result_of<F(Args...)>::type;
  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(std::forward<F>(f), std::forward<Args>(args)...));

  std::future<return_type> res = task->get_future();
  {
    std::unique_lock<std::mutex> lock(tasks_mtx);
    if (stop) {
      throw std::runtime_error("thread pool already stopped\n");
    }

    task_queue.emplace([task]() { (*task)(); });
  }
  cv.notify_one();
  return res;
}

}  // namespace cppserver_core

#endif