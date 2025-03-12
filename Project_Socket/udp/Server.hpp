#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <string>
#include <functional>
#include "log.hpp"

const static int defaultport = 8080;
const static std::string defaultip = "127.0.0.1";
const int B_SIZE = 1024;
static Log log;

// 三种写法
// typedef std::string(*func_t)(std::string);
// typedef std::function<std::string(std::string)> func_t;
using func_t = std::function<std::string(std::string)>;


// UDP面向数据报，无连接，不可靠，只需要创建套接字并且bind即可
class UDPServer{
public:
    UDPServer(const int& port = defaultport, const std::string& ip = defaultip)
        :_port(port)
        ,_ip(ip)
    {}
    void Init(){
        // 创建套接字
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(_sockfd < 0){
            log(Error, "sockfd error");
        }

        // 绑定地址和端口
        // 构造地址结构
        struct sockaddr_in local;  //  -> sockaddr类型的衍生
        // 初始化地址结构的内容
        memset(&local, 0, sizeof(local));
        // 地址结构填写(三件套：协议，端口，ip)
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        struct in_addr addr;
        if(inet_aton(_ip.c_str(), &addr) == 0){
            log(Error, "string ip to binary ip error");
        }
        local.sin_addr = addr;
        int rb = bind(_sockfd, (const struct sockaddr*)&local, sizeof(local));
        if(rb != 0){
            log(Error, "bind error");
        }
        std::cout << "UDP Server waiting for serve" << std::endl;
        fflush(stdout);
    }

    void Run(func_t fun){
        char buf[B_SIZE];
        while(1){
            struct sockaddr_in clisock;
            socklen_t socklen = sizeof(clisock);
            int recvsize = recvfrom(_sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&clisock, &socklen);
            if(recvsize < 0){
                log(Error, "recv message error!");
            }
            buf[recvsize] = '\0';
            std::string sbuf = buf;
            std::cout << "Server get a message: " << sbuf << std::endl;
            std::string backbuf = fun(sbuf);
            int sent_len = sendto(_sockfd, backbuf.c_str(), backbuf.size()+1, 0, (const struct sockaddr*)& clisock, socklen);
            if(sent_len < 0){
                log(Error, "send back error!");
            }
        }
    }

    ~UDPServer(){
        if(_sockfd > 0){
            close(_sockfd);
        }
    }


private:
    int _sockfd;
    std::string _ip;
    int _port;
};