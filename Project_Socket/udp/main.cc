#include <iostream>
#include "Server.hpp"


void Usage(const char* argv){
    std::cout << "please enter like:\"" << argv << ' ' << "8080(port)\"" << std::endl;
    std::flush(std::cout);

}

std::string ProcessString(std::string input){
    std::string ret = "[[[[";
    ret+=input;
    ret+="]]]]";
    return ret;
}

// ./server port
int main(int argc, char* argv[]){
    if(argc != 2){
        Usage(argv[0]);
        exit(0);
    }
    int setport = std::stoi(argv[1]);
    UDPServer server(setport);
    server.Init();
    server.Run(ProcessString);


    return 0;
}