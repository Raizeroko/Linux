#include "log.hpp"

void testLog(){
    Log log;
    log(Debug, "print: %d + %d = %d", 3, 5, 8);
}


int main(){
    testLog();
}