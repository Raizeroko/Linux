#include <sys/select.h>
#include <unistd.h>
#include "Socket.hpp"
#include "log.hpp"

const static std::string DEFAULT_IP = "0.0.0.0";
const static int DEFAULT_PORT = 8888;
const static int MAX_IO_NUM = (sizeof(fd_set) * 8);
const static int DEFAULT_FD = -1;


class SelectServer{
public:
    SelectServer(int port = DEFAULT_PORT, std::string ip = DEFAULT_IP)
        :_port(port)
        ,_ip(ip)
    {
        _listensock = new Socket();
        for(int i = 0; i < MAX_IO_NUM; i++){
            _sockfds[i] = DEFAULT_FD;
        }
    }

    void Init()
    {
        _listensock->CreateSocket();
        _listensock->Bind(_port);
        _listensock->Listen();
    }

    void Receiver(int fd, int pos){
        char buffer[1024];
        int rr = read(fd, buffer, sizeof(buffer));
        if(rr > 0){
            buffer[rr] = '\0';
            std::cout << "[client say]@ " << buffer;
            std::string echo_string = "[tcpserver echo]# ";
            echo_string += buffer;
            write(fd, echo_string.c_str(), echo_string.size());

        }
        else if(rr == 0){
            log(Debug, "client exit connect");
            close(fd);
            _sockfds[pos] = DEFAULT_FD;
        }
        else{
            log(Error, "read error");
            close(fd);
            _sockfds[pos] = DEFAULT_FD;
        }
        
    }



    // 进行tcp的accept，同时将accept的fd加入到监视文件描述符的辅助数组中
    void Accepter(){
        std::string ip;
        uint16_t port;
        int newfd = _listensock->Accept(&ip, &port);
        // 第一个已经是listenfd所以跳过
        int i = 1;
        for(int i = 1; i < MAX_IO_NUM; i++){
            if(_sockfds[i] == DEFAULT_FD){
                break;
            }
        }
        if(i == MAX_IO_NUM){
            log(Warning, "server is full, close %d now!", newfd);
            close(newfd);
        }
        else{
            _sockfds[i] = newfd;
            log(Info, "get a new link");
            PrintFd();
        }
    }


    void Despatcher(fd_set readfds){
        for(int i = 0; i < MAX_IO_NUM; i++){
            int sockfd = _sockfds[i];
            if(sockfd == DEFAULT_FD) continue;

            if(FD_ISSET(sockfd, &readfds)){
                if(sockfd == _listensock->GetSockfd()){
                    Accepter();
                }
                else{
                    Receiver(sockfd, i);
                }
            }
        }
    }



    void Start()
    {   
        // 默认_sockfdszhon
        _sockfds[0] = _listensock->GetSockfd();
        
        while(1){
            // 要关注的读事件fd集
            fd_set readfds;
            FD_ZERO(&readfds);
            int maxfd = _sockfds[0];
            // 将fd放入readfd集
            for(int i = 0; i < MAX_IO_NUM; i++){
                if(_sockfds[i] == DEFAULT_FD){
                    continue;
                }
                FD_SET(_sockfds[i], &readfds);
                if(_sockfds[i] > maxfd){
                    maxfd = _sockfds[i];
                }
            }

            // timeval表示多久监听一次，两个字段一个单位是s,y一个是us
            struct timeval timeout = {1, 0};
            int ret = select(maxfd+1, &readfds, NULL, NULL, &timeout);
            if(ret < 0){
                log(Fatal, "select error");
            }
            else if(ret == 0){
                // log(Info, "waiting for link......");
            }
            else{
                Despatcher(readfds);
            }
            
        }   
    }
    void PrintFd()
    {
        std::cout << "online fd list: ";
        for (int i = 0; i < MAX_IO_NUM; i++)
        {
            if (_sockfds[i] == DEFAULT_FD)
                continue;
            std::cout << _sockfds[i] << " ";
        }
        std::cout << std::endl;
    }
    ~SelectServer()
    {
        _listensock->Close();
        delete _listensock;
    }
private:
    Socket* _listensock;
    int _port;
    std::string _ip;

    // select中存放所有监视文件描述符的辅助数组
    int _sockfds[MAX_IO_NUM];
};