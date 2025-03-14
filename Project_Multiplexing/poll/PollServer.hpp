#include <string>
#include <poll.h>
#include <vector>


#include "Socket.hpp"

static const int DEFAULT_PORT = 7980; 
static const int MAX_POLL_NUM = 1024;
const static int DEFAULT_FD = -1;


class PollServer{
public:
    PollServer(int port = DEFAULT_PORT)
        :_port(port)
    {
        for(int i = 0; i < MAX_POLL_NUM; i++){
            _pollfds[i].fd = DEFAULT_FD;
        }
    }
    ~PollServer()
    {}
    void Init()
    {
        _listensock.CreateSocket();
        _listensock.Bind(_port);
        _listensock.Listen();
    }

    void Accepter(){
        uint16_t client_port;
        std::string client_ip;
        int newfd = _listensock.Accept(&client_ip, &client_port);
        int pos = 1;
        while(pos < MAX_POLL_NUM){
            if(_pollfds[pos].fd == DEFAULT_FD){
                break;
            }
            pos++;
        }
        if(pos == MAX_POLL_NUM){
            // 可以对pollfds增容
            log(Error, "pollfds already full");
            exit(-1);
        }
        else{
            _pollfds[pos].fd = newfd;
            _pollfds[pos].events = POLLIN; 
            _pollfds[pos].revents = 0;
        }
        
    }

    void Connecter(int pos){
        int connectfd = _pollfds[pos].fd;
        char buffer[1024];
        int rr = read(connectfd, buffer, sizeof(buffer)); 
        if(rr > 0){
            buffer[rr] = '\0';
            std::cout << "[client say]@ " << buffer;
            std::string echo_string = "[tcpserver echo]# ";
            echo_string += buffer;
            write(connectfd, echo_string.c_str(), echo_string.size());
        }
        else if(rr == 0){
            log(Info, "exit connect");
            close(connectfd);
            _pollfds[pos].fd = DEFAULT_FD;
            _pollfds[pos].events = 0;
            _pollfds[pos].revents = 0;
        }
        else{
            log(Error, "read error");
            close(connectfd);  
            _pollfds[pos].fd = DEFAULT_FD;
            _pollfds[pos].events = 0;
            _pollfds[pos].revents = 0; 
        }

    }

    void Despatcher(){
        for(int i = 0; i < MAX_POLL_NUM; i++){
            int fd = _pollfds[i].fd;
            if(fd == DEFAULT_FD) continue;
            if (_pollfds[i].revents & POLLIN){
                if(fd == _listensock.GetSockfd()){
                    Accepter();
                    
                }
                else{
                    Connecter(i);
                }
            }
        }
    }


    void Start()
    {
        struct pollfd listen_poll;
        listen_poll.fd = _listensock.GetSockfd();
        listen_poll.events = POLLIN;
        int timeout = 3000;
        _pollfds[0] = listen_poll;
        while(1){
            int rp = poll(_pollfds, MAX_POLL_NUM, -1);
            if(rp < 0){
                log(Error, "poll error");
            }
            else if(rp == 0){
                log(Info, "Waiting for new link");
            }
            else{
                Despatcher();
            }
        }
    }
    void Close()
    {
        int listenfd = _listensock.GetSockfd();
        if(listenfd > 0){
            close(listenfd);
        }
    }
private:
    Socket _listensock;
    struct pollfd _pollfds[MAX_POLL_NUM];
    int _port;
};