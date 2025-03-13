#include "SelectServer.hpp"
#include <iostream>

int main(){
    std::unique_ptr<SelectServer> server(new SelectServer);
    server->Init();
    server->Start();

    return 0;
}