#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <stdio.h>
#include <cstring>
#include "log.hpp"

void Usage(const char* argv){
    std::cout << "please enter like:\"" << argv << ' ' << "192.168.2.0(ip)" << ' ' <<"8080(port)\"" << std::endl;
    std::flush(std::cout);

}


int main(int argc, char* argv[]){
    if(argc != 3){
        Usage(argv[0]); 
        exit(0);
    }
    Log log;

    // socket创建
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(client_fd < -1){
        log(Error, "client sock create error");
    }

    // client 要bind吗？要！只不过不需要用户显示的bind！一般有OS自由随机选择！
    // 一个端口号只能被一个进程bind，对server是如此，对于client，也是如此！
    // 其实client的port是多少，其实不重要，只要能保证主机上的唯一性就可以！
    // 系统什么时候给我bind呢？首次发送数据的时候
    
    // 目标服务器
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
    serveraddr.sin_port = htons(std::stoi(argv[2]));


    while(1){
        std::cout << "please enter@";
        std::string message;
        getline(std::cin, message);
        int s = sendto(client_fd, message.c_str(), message.size() + 1, 0, (const sockaddr*)&serveraddr, sizeof(serveraddr));
        if(s < 0){
            log(Error, "send error");
        }


        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        char buffer[1024];
        recvfrom(client_fd, buffer, sizeof(buffer) - 1, 0, (sockaddr*)& tmp, &len);
        std::cout << "get message from server:" << buffer << std::endl;
    }
    
}