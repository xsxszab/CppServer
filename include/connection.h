#ifndef CONNECTION_H
#define CONNECTION_H

#include <functional>

class EventLoop;
class Socket;
class Channel;

class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(Socket *)> deleteConnectionCallBack;

public:
    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();

    void setDeleteConnectionCallBack(std::function<void(Socket *)>);

    void echo(int sockfd);
};

#endif