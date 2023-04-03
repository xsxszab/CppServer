#include <vector>

#include "event_loop.h"
#include "channel.h"
#include "epoll_class.h"
#include "thread_pool.h"

EventLoop::EventLoop() : ep(nullptr), threadpool(nullptr), quit(false)
{
    ep = new Epoll();
    threadpool = new ThreadPool();
}

EventLoop::~EventLoop()
{
    delete ep;
}

void EventLoop::loop()
{
    while (!quit)
    {
        std::vector<Channel *> channels = ep->poll();
        for (auto it = channels.begin(); it != channels.end(); it++)
        {
            (*it)->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    ep->updateChannel(channel);
}

void EventLoop::addThread(std::function<void()> func)
{
    threadpool->add(func);
}
