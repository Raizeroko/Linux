#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>

using namespace std;

#define LABLE "$"
#define DELIM " "
#define LINE_SIZE 1024
#define ARGC_SIZE 32
#define EXIT_CODE 44

char hostname[32];
char pwd[LINE_SIZE];
int lastCode = 0; 
extern char **environ;


const char *GetUsrName()
{
    return getenv("USER");
}

const char *GetHostName()
{

    if(gethostname(hostname, sizeof(hostname)) == 0)
    {
        // printf("Hostname: %s\n", hostname);
        return hostname;
    }
    else
    {
        perror("Hostname error");
        return "-1";
    }
}

const char* GetModifier()
{
    return getenv("CONDA_PROMPT_MODIFIER");
}

void GetPWD()
{
    getcwd(pwd, sizeof(pwd)-1);
}

void interaction(char* commandline, int size)
{
    GetPWD();
    // 字符串和宏之间需要有空格
    printf("%s\033[1;32m%s@%s\033[0m:\033[1;34m%s\033[0m" LABLE " ",GetModifier(), GetUsrName(), GetHostName(), pwd);
    // 命令行输入
    char* s = fgets(commandline, size, stdin);
    assert(s != NULL);
    // 防止s没有被使用的报错
    (void) s;
    // 去除'\n'
    commandline[strlen(commandline)-1] = '\0';

}

// int splitString(char* commandline, char* argv[]) {
//     int i = 0;
//     char* token = strtok(commandline, DELIM);
//     while (token != NULL && i < ARGC_SIZE - 1) {  // 确保 argv 不越界
//         argv[i++] = token;
//         token = strtok(NULL, DELIM);
//     }
//     for(int j=0; j<i;j++)
//     {
//         cout<<argv[j];
//     }
//     argv[i] = NULL;  // 将最后一个元素设置为 NULL，便于 execvp 调用
//     return i;
// }

int splitString(char* commandline, char* argv[ARGC_SIZE])
{
    int i = 0;
    argv[i++] = strtok(commandline, DELIM);
    // 重复划分同一字符串第二次需要传NULL
    while((argv[i++] = strtok(NULL, DELIM)));
    argv[i] = NULL;
    // for(int j=0; j<i;j++)
    // {
    //     cout<<argv[j];
    // }
    return i-1 ;
}

void NormalExcute(char *argv[])
{
    pid_t id = fork();
        if(id < 0){
            perror("fork error");
            return;
        }
        else if(id == 0){
            // 子进程

            
            execvpe(argv[0], argv, environ);
            // 进程代码替换失败
            exit(EXIT_CODE);
        }
        else{
            // 父进程
            int status=0;
            pid_t rid = waitpid(id, &status, 0);
            if(rid = id){
                // 保存最近的退出码
                lastCode = WEXITSTATUS(status);
                cout<< "return success" << endl ;
            }
        }
}

int BuildCommand(char* argv[], int argc)
{
    if(argc == 2 && strcmp(argv[0], "cd") == 0){
        chdir(argv[1]);
        GetPWD();
        sprintf(getenv("PWD"), "%s", pwd);
        return 1;
    }
    if(strcmp(argv[0], "ls") == 0){
        argv[++argc] = "--color";
        argv[argc] = NULL;

    }
    return -1;
    
}


int main()
{
    char commandline[LINE_SIZE];
    char *argv[ARGC_SIZE];

    int quit = 0;
    while(!quit){
        // 命令行交互
        interaction(commandline, LINE_SIZE);

        // 分割命令
        int argc = splitString(commandline, argv);
        if(argc == 0) continue;

        // 内建命令pwd
        BuildCommand(argv, argc);
        

        // 命令执行
        NormalExcute(argv);
    }   
        
    return 0;
}