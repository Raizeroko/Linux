#include "Init.hpp"
#include "log.hpp"

int main()
{
    Init init;
    Log log;    
    log.Enable(Onefile);        // Screen/Onefile/Class

    // 打开管道文件
    int fd = open(FIFO_FILE, O_RDONLY);
    if(fd < 0){
        log(Fatal, "error string: %s, error code: %d", strerror(errno), errno);
        exit(FIFO_OPEN_ERR);
    }

    log(Info, "Finished open server: %s, error code: %d", strerror(errno), errno);
    log(Debug, "Finished open server: %s, error code: %d", strerror(errno), errno);
    log(Warning, "Finished open server: %s, error code: %d", strerror(errno), errno);
    log(Error, "Finished open server: %s, error code: %d", strerror(errno), errno);
    log(Fatal, "Finished open server: %s, error code: %d", strerror(errno), errno);



    // 开始通信
    while(true){
        char buf[1024] = {0};
        int x = read(fd, buf, sizeof(buf));
        if(x > 0){
            buf[x] = 0;
            cout << "client: "<< buf <<endl;
        }
        else if(x == 0){
            log(Debug, "client exited: %s, error code: %d", strerror(errno), errno);
            break;
        }
        else{
            break;
        }
    }
    
    close(fd);

    return 0;
}