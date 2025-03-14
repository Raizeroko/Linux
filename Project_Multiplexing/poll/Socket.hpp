#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <arpa/inet.h>

#include "log.hpp"

enum{
    SOCK_ERROR = 2,
    BIND_ERROR,
    LISTEN_ERROR 
};


static const int BACKLOG_SIZE = 10;

class Socket{
public:
    Socket(){}
    ~Socket(){
        if (_sockfd > 0) {
            close(_sockfd);
        }
    }

    void CreateSocket()
    {
        _sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if(_sockfd < 0){
            log(Fatal, "socker error, %s: %d", strerror(errno), errno);
            exit(SOCK_ERROR);
        }
        int opt = 1;
        setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
        log(Info, "create socket success");
    }

    void Bind(uint16_t port)
    {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        // 绑定到本地所有可用端口
        addr.sin_addr.s_addr = INADDR_ANY;
        int rb = bind(_sockfd, (const sockaddr*)& addr, sizeof(addr));
        if(rb < 0){
            log(Fatal, "bind error, %s: %d", strerror(errno), errno);
            exit(BIND_ERROR);

        }
        log(Info, "bind socket success");

    }

    void Listen()
    {
        int lb = listen(_sockfd, BACKLOG_SIZE);
        if(lb < 0){
            log(Fatal, "listen error, %s: %d", strerror(errno), errno);
            exit(LISTEN_ERROR);
        }
        log(Info, "listen socket success");

    }

    // 输出型参数(clientip, clientport)
    int Accept(std::string *clientip, uint16_t *clientport)
    {
        struct sockaddr_in addr;
        socklen_t addrlen = sizeof(addr);
        int accpet_fd = accept(_sockfd, (struct sockaddr*)& addr, &addrlen);
        if(accpet_fd < 0){
            log(Warning, "accept error, %s: %d", strerror(errno), errno);
            return -1;
        }
        *clientport = (addr.sin_port);
        *clientip = inet_ntoa(addr.sin_addr);
        return accpet_fd;
    }

    bool Connect(const std::string &ip, const uint16_t &port)
    {
        struct sockaddr_in connect_addr;
        memset(&connect_addr, 0, sizeof(connect_addr));
        connect_addr.sin_family = AF_INET;
        connect_addr.sin_port = htons(port);
        // 绑定到本地所有可用端口
        connect_addr.sin_addr.s_addr = inet_addr(ip.c_str());
        int cr = connect(_sockfd, (const sockaddr*)&connect_addr, sizeof(connect_addr));
        if(cr < 0){
            log(Warning, "connet error, %s: %d", strerror(errno), errno);
            return false;
        }
        log(Info, "connect success");
        
        return true;

    }
    void Close()
    {
        close(_sockfd);
    }

    int GetSockfd()
    {
        return _sockfd;
    }

private:
    int _sockfd;
};