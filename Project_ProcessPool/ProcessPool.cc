#include "Task.hpp"


const int tasknum = 5;
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

    int _fd;
    pid_t _id;
    string _processname; 
};

void excuteTask()
{
    int which;
    while(true){
        // process will be blocked
        int readBytes = read(0, &which, sizeof(which));
        if(readBytes == 0) break;
        cout << "readBytes: " << readBytes << endl;
        Task t;
        printf("child: %d ->", getpid());
        t[which];
    }
    
}


void initProcessPool(vector<channel>& channels){
    vector<int> oldfd;
    for(int i = 0; i < Task()._tasknum; i++){
        int pipefd[2];
        pipe(pipefd);
        pid_t id = fork();
        if(id < 0){
            cout << "fork error" << endl;
            exit(-1);
        }
        if(id == 0){
            // child
            // close father write pipe
            for(auto &e: oldfd)
            {
                close(e);
            }

            // close pipe write
            close(pipefd[1]);
            // redirection to no.0 filefd, this will close stdin
            dup2(pipefd[0], 0);            
            excuteTask();
            exit(0);
        }
        // father
        // close pipe read
        close(pipefd[0]);
        string processname = "process - " + to_string(i);
        channel c(pipefd[1], id, processname);
        channels.push_back(c);
        oldfd.push_back(pipefd[1]);
    }
}

void menu()
{
    cout << "*********************************" << endl;
    cout << "* 1.野怪刷新         2.小兵刷新 *" << endl;
    cout << "* 3.释放技能         4.对局开始 *" << endl;
    cout << "* 5.对局结束         6.退出游戏 *" << endl;
    cout << "*********************************" << endl;

}


void controlProcess(vector<channel>& channels){
    menu();
    int task = 0;
    while(true){
        int c = rand() % channels.size();
        cout << "选择任务:";
        cin >> task;
        if(task == 6)
        {
            break;  
        } 
        printf("father send: %d to channel[%d]\n", task, c);
        write(channels[c]._fd, &task, sizeof(task));
        sleep(1);
    }
    cout << "finish control"<<endl;

}

void quitProcess(vector<channel>& channels){
    for(int i = 0; i < channels.size(); i++)
    {
        close(channels[i]._fd);
        cout << "finish close: "<< channels[i]._fd << endl;
        waitpid(channels[i]._id, nullptr, 0);
        cout << "finish wait: "<< channels[i]._id << endl;   
    
    }

    cout << "Finish quit" <<endl;
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
    quitProcess(channels);
    return 0;

}