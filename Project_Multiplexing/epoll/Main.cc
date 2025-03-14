#include "EpollServer.hpp"
#include <iostream>

int main(){
    std::unique_ptr<EpollServer> server_ptr(new EpollServer);
    server_ptr->Init();
    server_ptr->Start();

    return 0;
}