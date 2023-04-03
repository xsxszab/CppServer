#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> task_queue;
    std::mutex tasks_mtx;
    std::condition_variable cv;

    bool stop;

public:
    ThreadPool(int num_thread = 8);
    ~ThreadPool();

    void add(std::function<void()>);
};

#endif