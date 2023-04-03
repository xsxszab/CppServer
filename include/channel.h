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
    uint32_t ready;

    bool in_epoll;
    bool use_threadpool;

    std::function<void()> read_callback;
    std::function<void()> write_callback;

public:
    Channel(EventLoop *_el, int _fd);
    ~Channel();

    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getReady();
    bool inEpoll();
    void setInEpoll();

    void useET();

    void setReady(uint32_t);

    void setReadCallBack(std::function<void()>);
    void handleEvent();

    void setUseThreadPool(bool use = true);
};

#endif