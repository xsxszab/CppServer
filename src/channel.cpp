#include "channel.h"
#include "epoll_class.h"
#include "event_loop.h"

#include <unistd.h>

Channel::Channel(EventLoop *_el, int _fd)
    : el(_el), fd(_fd), events(0), ready(0), in_epoll(false), use_threadpool(true) {}

Channel::~Channel()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

int Channel::getFd()
{
    return fd;
}

uint32_t Channel::getEvents()
{
    return events;
}

uint32_t Channel::getReady()
{
    return ready;
}

bool Channel::inEpoll()
{
    return in_epoll;
}

void Channel::setInEpoll()
{
    in_epoll = true;
}

void Channel::useET()
{
    events |= EPOLLET;
    el->updateChannel(this);
}

void Channel::setReady(uint32_t _ev)
{
    ready = _ev;
}

void Channel::setReadCallBack(std::function<void()> func)
{
    read_callback = func;
}

void Channel::handleEvent()
{
    if (ready & (EPOLLIN | EPOLLPRI))
    {
        if (use_threadpool)
        {
            el->addThread(read_callback);
        }
        else
        {
            read_callback();
        }
    }

    if (ready & EPOLLOUT)
    {
        if (use_threadpool)
        {
            el->addThread(write_callback);
        }
        else
        {
            write_callback();
        }
    }
}

void Channel::setUseThreadPool(bool use)
{
    use_threadpool = use;
}

void Channel::enableReading()
{
    events |= EPOLLIN | EPOLLET;
    el->updateChannel(this);
}
