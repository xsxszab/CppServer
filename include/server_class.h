#ifndef SERVER_CLASS_H
#define SERVER_CLASS_H

#include <unordered_map>

class EventLoop;
class Socket;
class Acceptor;
class Connection;

class Server
{
private:
    EventLoop *loop;
    Acceptor *acceptor;
    std::unordered_map<int, Connection *> connections;

public:
    Server(EventLoop *);
    ~Server();

    void newConnection(Socket *);
    void deleteConnection(Socket *);
};

#endif
