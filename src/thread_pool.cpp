#include "thread_pool.h"

ThreadPool::ThreadPool(int num_thread) : stop(false)
{
    for (int i = 0; i < num_thread; i++)
    {
        threads.emplace_back(std::thread([this]()
                                         {
            while(true) 
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    cv.wait(lock, [this](){
                        return stop || !task_queue.empty();
                    });
                    if(stop && task_queue.empty()) return;
                    task = task_queue.front();
                    task_queue.pop();
                }
                task();
            } }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    cv.notify_all();
    for (auto &thread : threads)
    {
        if (thread.joinable())
            thread.join();
    }
}
