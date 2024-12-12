#include "comm.hpp"
#include "log.hpp"
#include <iostream>
 
using namespace std;

int main(){
    Log log;

    // 以只写的方式打开管道
    int fd = open(FIFO_FILE, O_WRONLY | O_EXCL);
    if(fd < 0) 
        log(Fatal, "open pipe error: %s", strerror(errno));

    // 获取
    key_t key = getSharedKey();
    if(key == -1) 
        log(Fatal, "create key error: %s", strerror(errno));

    int shmid = getSharedMemory(key);
    if(shmid == -1)
        log(Fatal, "shared memory error: %s", strerror(errno));
    // 挂接
    char* str = (char*) shmat(shmid, NULL, 0);
    log(Info, "processB finish prepare ! shmid: %d", shmid);
    
    // 通信
    while(true){
        cout << "Wait for message: "<< endl;
        fgets(str, SMEM_SIZE, stdin);
        int writef = write(fd, "M", 1);
        if(writef < 0 ){
            log(Fatal, "write error: %s", strerror(errno));
            break;
        }
    }

    // 关闭管道
    int closef = close(fd);
    if(closef == -1)
        log(Fatal, "close error: %s", strerror(errno));
    
    // 脱离
    int detach = shmdt(str);
    if(detach == -1)
        log(Fatal, "detach error: %s", strerror(errno));

}
