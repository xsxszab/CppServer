#include <iostream>
#include <functional>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "server_class.h"
#include "socket_class.h"
#include "inet_address.h"
#include "event_loop.h"
#include "channel.h"
#include "connection.h"
#include "acceptor.h"

#define MAX_EVENTS 1024
#define BUFFER_SIZE 1024

Server::Server(EventLoop *_loop) : loop(_loop), acceptor(nullptr)
{
    acceptor = new Acceptor(loop);
    acceptor->setNewConnectionCallBack(std::bind(&Server::newConnection, this, std::placeholders::_1));
}

Server::~Server()
{
    delete acceptor;
    // std::cout << "server closed" << std::endl;
}

void Server::newConnection(Socket *sock)
{
    Connection *conn = new Connection(loop, sock);
    conn->setDeleteConnectionCallBack(std::bind(&Server::deleteConnection, this, std::placeholders::_1));
    connections[sock->getFd()] = conn;
}

void Server::deleteConnection(Socket *sock)
{
    Connection *conn = connections[sock->getFd()];
    connections.erase(sock->getFd());
    delete conn;
}
