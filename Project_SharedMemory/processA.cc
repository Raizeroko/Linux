#include "comm.hpp"
#include "log.hpp"

int main(){
    // 创建命名管道
    Init init;
    Log log;
    // 创建
    key_t key = getSharedKey();
    if(key == -1) 
        log(Fatal, "create key error: %s", strerror(errno));
    
    int shmid = createSharedMemory(key);
    if(shmid == -1)
        log(Fatal, "shared memory error: %s", strerror(errno));
    

    // 挂接
    char* str = (char*) shmat(shmid, NULL, 0);

    // 以只读方式打开管道
    // 以（O_RDONLY）打开管道时，如果没有其他进程以写方式打开管道，open 将默认阻塞。
    int fd = open(FIFO_FILE, O_CREAT | O_RDONLY);
    if(fd < 0) 
        log(Fatal, "open pipe error: %s", strerror(errno));

    log(Info, "processA finish prepare ! shmid: %d", shmid);

    // 通信 
    while(true){
        char m;
        ssize_t flag= read(fd, &m, 1);
        if(flag == 0 ){
            log(Info, "processB have been exit");
            break;
        }
        if(flag < 0 ){
            log(Fatal, "read error: %s", strerror(errno));
            break;
        }
        cout << "Get message: " << str;
        
    }
    
    
    // 关闭命名管道
    int closef = close(fd);
    if(closef == -1)
        log(Fatal, "close error: %s", strerror(errno));
    
    // 脱离
    int detach = shmdt(str);
    if(detach == -1)
        log(Fatal, "detach error: %s", strerror(errno));

    // 删除
    int control = shmctl(shmid, IPC_RMID, NULL);
    if(control == - 1)
        log(Fatal, "delete shared memory error: %s", strerror(errno));
    log(Info, "processA Finish exit");

}
