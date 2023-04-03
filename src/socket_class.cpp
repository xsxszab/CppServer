#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <cstring>

#include "inet_address.h"
#include "utilfunc.h"
#include "socket_class.h"

Socket::Socket() : fd(-1)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}

Socket::Socket(int _fd) : fd(_fd)
{
    errif(fd == -1, "socket create error");
}

Socket::~Socket()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress *_addr)
{
    struct sockaddr_in addr = _addr->getAddr();
    socklen_t addr_len = _addr->getAddrLen();
    errif(::bind(fd, (sockaddr *)&addr, addr_len) == -1, "socket bind error");
    _addr->setAddr(addr, addr_len);
}

void Socket::listen()
{
    errif(::listen(fd, SOMAXCONN) == -1, "socket listen error");
}

void Socket::setnonblocking()
{
    ::fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress *_addr)
{
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    memset(&addr, '\0', sizeof(addr));
    int clnt_fd;
    if (fcntl(fd, F_GETFL) & O_NONBLOCK)
    {
        while (true)
        {
            clnt_fd = ::accept(fd, (sockaddr *)&addr, &addr_len);
            if (clnt_fd == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            {
                continue;
            }
            else if (clnt_fd == -1)
            {
                errif(true, "socket accept error");
            }
            else
                break;
        }
    }
    else
    {
        clnt_fd = ::accept(fd, (sockaddr *)&addr, &addr_len);
        errif(clnt_fd == -1, "socket accept error");
    }

    _addr->setAddr(addr, addr_len);
    return clnt_fd;
}

int Socket::getFd()
{
    return fd;
}
