#include "channel.h"
#include "epoll_class.h"
#include "event_loop.h"

Channel::Channel(EventLoop *_el, int _fd) : el(_el), fd(_fd), events(0), revents(0), in_epoll(false)
{
}

Channel::~Channel()
{
}

int Channel::getFd()
{
    return fd;
}

uint32_t Channel::getEvents()
{
    return events;
}

uint32_t Channel::getRevents()
{
    return revents;
}

bool Channel::inEpoll()
{
    return in_epoll;
}

void Channel::setInEpoll()
{
    in_epoll = true;
}

void Channel::setRevents(uint32_t _ev)
{
    events = _ev;
}

void Channel::setCallBack(std::function<void()> func)
{
    callback_func = func;
}

void Channel::handleEvent()
{
    callback_func();
}

void Channel::enableReading()
{
    events = EPOLLIN | EPOLLET;
    el->updateChannel(this);
}
