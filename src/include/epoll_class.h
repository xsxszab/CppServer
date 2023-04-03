#ifndef EPOLL_CLASS_H
#define EPOLL_CLASS_H

#include <sys/epoll.h>
#include <vector>

class Channel;

class Epoll
{
private:
    int epfd;
    struct epoll_event *events;

public:
    Epoll();
    ~Epoll();

    // void add_fd(int fd, uint32_t op);
    std::vector<Channel *> poll(int timeout = -1);

    void updateChannel(Channel *);
};

#endif