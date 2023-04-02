#ifndef CHANNEL_H
#define CHANNEL_H

#include <sys/epoll.h>
#include <functional>

class Epoll;
class EventLoop;

class Channel
{
private:
    EventLoop *el;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool in_epoll;
    std::function<void()> callback_func;

public:
    Channel(EventLoop *_el, int _fd);
    ~Channel();

    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool inEpoll();
    void setInEpoll();

    void setRevents(uint32_t);

    void setCallBack(std::function<void()>);
    void handleEvent();
};

#endif