#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include "log.hpp"
#include "Socket.hpp"


const static std::string DEFAULT_IP = "127.0.0.1";
const static size_t DEFAULT_PORT = 8080;


class TCPServer{
public:
    TCPServer(size_t port = DEFAULT_PORT, std::string ip = DEFAULT_IP)
        :_ip(ip)
        ,_port(port)
    {}

    // void Init(){
    //     // socket
    //     _listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //     if(_listenfd < 0){
    //         log(Error, "sockfd create error");
    //     }
    //     int optval = 1;
    //     setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval));
    //     // bind
    //     struct sockaddr_in serveraddr;
    //     memset(&serveraddr, 0, sizeof(serveraddr));
    //     serveraddr.sin_family = AF_INET;
    //     serveraddr.sin_port = htons(_port);
    //     serveraddr.sin_addr.s_addr = inet_addr(_ip.c_str());
    //     int br = bind(_listenfd, (const sockaddr *)&serveraddr, sizeof(serveraddr));
    //     if(br < 0){
    //         log(Error, "bind error");
    //     }
    //     // listen
    //     int lr = listen(_listenfd, BACKLOG_SIZE);
    //     if(lr < 0){
    //         log(Error, "listen error");
    //     }

    //     std::cout << "server start, waitting for client" << std::endl;
    // }

    void Init(){
        _listensock.CreateSocket();
        _listensock.Bind(_port);
        _listensock.Listen();
    }

    void Start(){
        while(1){
            std::string clientip;
            uint16_t clientport = 0;
            int sockfd = _listensock.Accept(&clientip, &clientport);
            if(sockfd < 0){
                log(Error, "accpet error");
            }
            pthread_t tid;
            pthread_create(&tid, nullptr, routine ,&sockfd);
        }
    }
    
    // void Start(){
    //     while(1){
    //         struct sockaddr_in clientaddr;
    //         socklen_t clientlen = sizeof(clientaddr);
    //         int sockfd = accept(_listenfd, (sockaddr*)&clientaddr, &clientlen);
    //         if(sockfd < 0){
    //             log(Error, "accpet error");
    //         }

    //         // 一个进程
    //         // Service(sockfd);
    //         // close(sockfd);
            
    //         // 多进程
    //         // pid_t pid = fork();
    //         // if(pid == 0)
    //         // {
    //         //     // child
    //         //     close(_listenfd);
    //         //     if(fork() > 0) exit(0);
    //         //     Service(sockfd); //孙子进程， system 领养
    //         //     close(sockfd);
    //         //     exit(0);
    //         // }
    //         // else if(pid > 0){
    //         //     std::cout << "get a new link" << std::endl;
    //         //     close(sockfd);
    //         //     pid_t rid = waitpid(pid, nullptr, 0);

    //         // }
    //         // else{
    //         //     std::cout << "fork error" << std::endl;
    //         // }

    //         // 多线程
    //         pthread_t tid;
    //         pthread_create(&tid, nullptr, routine ,&sockfd);

    //     }
    // }

    static void* routine(void* arg){
        pthread_detach(pthread_self());
        int* sockfd = (int*)arg;
        Service(*sockfd);
        return nullptr;
    }

    static void Service(int sockfd)
    {
        char buffer[1024];
        while(1){
            ssize_t nr = read(sockfd, buffer, sizeof(buffer));
            if(nr > 0){
                buffer[nr] = '\0';
                std::cout << "[client say]@ " << buffer;
                std::string echo_string = "[tcpserver echo]# ";
                echo_string += buffer;
                write(sockfd, echo_string.c_str(), echo_string.size());

            }
            else if(nr == 0){
                log(Debug, "client exit connect");
                break;
            }
            else{
                log(Error, "read error");
                break;
            }

        }

    }

    ~TCPServer()
    {
        _listensock.Close();
    }
private:
    Socket _listensock;
    std::string _ip;
    size_t _port;

};