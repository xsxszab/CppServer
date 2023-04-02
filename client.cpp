#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <utilfunc.h>

#define BUFFER_SIZE 1024

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    errif(connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");
    while (true)
    {
        char buf[BUFFER_SIZE];
        bzero(&buf, sizeof(buf));
        std::cin >> buf;
        ssize_t bytes_write = write(sockfd, buf, sizeof(buf));
        if (bytes_write == -1)
        {
            std::cout << "socket closed, cannot write bytes" << std::endl;
            break;
        }
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            std::cout << "message from server: " << buf << std::endl;
        }
        else if (bytes_read == 0)
        {
            std::cout << "server socket disconnected" << std::endl;
            break;
        }
        else
        {
            close(sockfd);
            errif(true, "socket read error");
        }
    }
    close(sockfd);
    return 0;
}