#ifndef INET_ADDRESS_H
#define INET_ADDRESS_H

#include <arpa/inet.h>

class InetAddress
{
public:
    struct sockaddr_in addr;
    socklen_t addr_len;

    InetAddress();
    InetAddress(const char *ip, uint16_t port);
    ~InetAddress();

    void setAddr(sockaddr_in _addr, socklen_t _addr_len);

    sockaddr_in getAddr();
    socklen_t getAddrLen();
};

#endif