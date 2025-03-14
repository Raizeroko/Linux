#include "PollServer.hpp"
#include <iostream>

int main(){
    std::unique_ptr<PollServer> server(new PollServer);
    server->Init();
    server->Start();
    return 0;
}