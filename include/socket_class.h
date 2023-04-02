#ifndef SOCKET_CLASS_H
#define SOCKET_CLASS_H

class InetAddress;

class Socket
{
private:
    int fd;

public:
    Socket();
    Socket(int _fd);
    ~Socket();

    void bind(InetAddress *);
    void listen();
    void setnonblocking();

    int accept(InetAddress *);

    int getFd();
};

#endif