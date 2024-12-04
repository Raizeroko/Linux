#include "Task.hpp"

const int processnum = 5;

// 输入型参数：const &
// 输出型参数：*
// 输入输出型参数：&

// 先描述
class channel{
public:
    channel(int fd, pid_t id, string processname)
        :_fd(fd)
        ,_id(id)
        ,_processname(processname)
    {}

private:
    int _fd;
    pid_t _id;
    string _processname; 
};

void initProcessPool(vector<channel>& channels){
    for(int i = 0; i < processnum; i++){
        int pipefd[2];
        pipe(pipefd);
        pid_t id = fork();
        if(id < 0){
            cout << "fork error" << endl;
            exit(-1);
        }
        if(id == 0){
            // child
            // close pipe write
            close(pipefd[1]);
            // redirection to no.0 filefd, this will close stdin
            dup2(pipefd[0], 0);
            char buf[1024];
            // process will be blocked
            read(pipefd[0], buf, sizeof(buf));
            exit(0);
        }
        // father
        // close pipe read
        close(pipefd[0]);
        string processname = "process - " + to_string(i);
        channel c(pipefd[1], id, processname);
        channels.push_back(c);
    }
}


void controlProcess(vector<channel> channels){

}


int main()
{
    srand(time(nullptr));

    // 再组织
    vector<channel> channels;
    // 初始化
    initProcessPool(channels);
    // 控制子进程
    controlProcess(channels);
    // 清理收尾
    return 0;

}