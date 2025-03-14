#include "log.hpp"
#include "Socket.hpp"
#include "NoCopy.hpp"
#include "Epoller.hpp"

#include <unistd.h>
#include <sys/epoll.h>
static const uint16_t DEFAULT_PORT = 8080;

class EpollServer : public NoCopy
{
    static const int max_event = 10;

public:
    EpollServer(uint16_t port = DEFAULT_PORT)
        :_port(port)
    {
        _listen_sock = std::make_shared<Socket>();
        _epoller = std::make_shared<Epoller>();
    }
    void Init()
    {
        _listen_sock->CreateSocket();
        _listen_sock->Bind(_port);
        _listen_sock->Listen();
    }

    void Accepter()
    {
        std::string client_ip;
        uint16_t client_port;
        int newfd = _listen_sock->Accept(&client_ip, &client_port);
        if (newfd > 0){
            _epoller->Control(EPOLL_CTL_ADD, newfd, EPOLLIN);
            log(Info, "get a new link, client info@ %s:%d", client_ip.c_str(), client_port);
        }
    }

    void Connecter(int fd)
    {
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
            log(Info, "client quit, me too, close fd is : %d", fd);
            // 先从epoll中移除fd再关闭。
            _epoller->Control(EPOLL_CTL_DEL, fd, 0);
            close(fd);
        }
        else{
            log(Warning, "recv error: fd is : %d", fd);
            _epoller->Control(EPOLL_CTL_DEL, fd, 0);
            close(fd);

        }
    }

    void Despatcher(struct epoll_event* events, int envent_num){
        int listen_fd = _listen_sock->GetSockfd();
        for(int pos = 0; pos < envent_num; pos++){
            if(events[pos].data.fd == listen_fd){
                Accepter();
            }
            else{
                Connecter(events[pos].data.fd);
            }
        }
    }



    void Start()
    {
        // 创建epoll模型
        int listen_fd = _listen_sock->GetSockfd();

        // 监听哪个文件描述符，关心什么事。
        // 将关心的文件描述符，对应事件加入到epoll模型中
        _epoller->Control(EPOLL_CTL_ADD, listen_fd, EPOLLIN);
        struct epoll_event revnt[max_event];
        
        while(1){
            int n = _epoller->Wait(revnt, max_event);
            if(n > 0){
                Despatcher(revnt, n);
            }
            else if(n == 0){
                log(Info, "waiting for new link");
            }
            else{
                log(Error, "epoll wait error");
                exit(-1);
            }
        }

    }

    ~EpollServer()
    {}
private:
    std::shared_ptr<Socket> _listen_sock;
    std::shared_ptr<Epoller> _epoller;
    uint16_t _port;
};
