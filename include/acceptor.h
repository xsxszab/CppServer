#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;

class Acceptor
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;

    std::function<void(Socket *)> newConnectionCallBack;

public:
    Acceptor(EventLoop *_loop);
    ~Acceptor();

    void acceptConnection();

    void setNewConnectionCallBack(std::function<void(Socket *)>);
};

#endif
