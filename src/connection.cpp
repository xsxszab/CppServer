#include <cstring>
#include <iostream>
#include <unistd.h>

#include "connection.h"
#include "socket_class.h"
#include "buffer.h"
#include "utilfunc.h"
#include "channel.h"

#define BUFFER_SIZE 1024

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr), buffer(nullptr)
{
    channel = new Channel(loop, sock->getFd());
    channel->setCallBack(std::bind(&Connection::echo, this, sock->getFd()));
    channel->enableReading();

    buffer = new Buffer();
}

Connection::~Connection()
{
    delete channel;
    delete sock;
    delete buffer;
}

void Connection::setDeleteConnectionCallBack(std::function<void(Socket *)> func)
{
    deleteConnectionCallBack = func;
}

void Connection::echo(int sockfd)
{
    char buf[BUFFER_SIZE];
    while (true)
    {
        memset(&buf, '\0', sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            // std::cout << "message from client fd " << sockfd << ": " << buf << std::endl;
            // write(sockfd, buf, sizeof(buf));
            buffer->append(buf, bytes_read);
        }
        else if (bytes_read == -1 && errno == EINTR)
        {
            std::cout << "continue reading" << std::endl;
        }
        else if (bytes_read == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            std::cout << "finish reading once, errno: " << errno << std::endl;
            std::cout << "message from client: " << buffer->c_str() << std::endl;
            errif(write(sockfd, buffer->c_str(), buffer->size()) == -1, "socket write error");
            buffer->clearBuffer();
            break;
        }
        else if (bytes_read == 0)
        {
            std::cout << "EOF, client " << sockfd << " disconnected" << std::endl;
            deleteConnectionCallBack(sock);
            break;
        }
    }
}
