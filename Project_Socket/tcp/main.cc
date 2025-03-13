#include <iostream>
#include "TCPServer.hpp"


void Usage(const char* argv){
    std::cout << "please enter like:\"" << argv << ' ' << "8080(port)\"" << std::endl;
    std::flush(std::cout);

}



// ./server port
int main(int argc, char* argv[]){
    int setport = DEFAULT_PORT;
    if(argc == 1){
        std::cout << "use default port" << std::endl;
    }
    else if(argc == 2){
        int setport = std::stoi(argv[1]);
    }
    else{
        Usage(argv[0]);
        exit(0);
    }
    TCPServer server(setport);
    server.Init();
    server.Start();


    return 0;
}