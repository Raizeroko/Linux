#include "Socket.hpp"
#include <iostream>
#include <string>

void Usage(const char* argv){
    std::cout << "please enter like:\"" << argv << ' ' << "192.168.2.0(ip)" << ' ' <<"8080(port)\"" << std::endl;
    std::flush(std::cout);

}

int main(int argc, char* argv[]){
    if(argc != 3){
        Usage(argv[0]); 
        exit(0);
    }

    Socket sock;
    sock.CreateSocket();
    std::string serverip = argv[1];
    int serverport = std::stoi(argv[2]);
    std::cout << "before connect" << std::endl;

    bool cr = sock.Connect(serverip, serverport);
    std::cout << "connect ret" << std::endl;
    if(!cr)
    {
        std::cout << "connect error" << std::endl;
        exit(-1);
    }
    int client_fd = sock.GetSockfd();
    while(1){
        std::cout << "please enter@";
        std::string message;
        getline(std::cin, message);
        int w = write(client_fd, message.c_str(), message.size());
        if(w < 0){
            log(Error, "send error");
        }


        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        char buffer[1024];
        int r = read(client_fd, buffer, sizeof(buffer));
        if(r < 0){
            log(Error, "read error");
        }
        buffer[r] = '\0';
        std::cout << "get message from server:" << buffer << std::endl;
    }
    

}