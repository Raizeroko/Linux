#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

#define LABLE "$"
#define DELIM " "
#define LINE_SIZE 1024
#define ARGC_SIZE 32
#define EXIT_CODE 44

#define NONE -1
#define IN_RDIR 0
#define OUT_PDIR 1
#define APPEND_RDIR 2

char hostname[32];
char myenv[LINE_SIZE];
char pwd[LINE_SIZE];
int lastCode = 0; 
extern char **environ;
char* redirfilename = nullptr;
int redir = NONE;

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

void CheckRedir(char *cmd)
{
    char *pos = cmd;
    while(*pos){
        if(*pos == '>'){
            if(*(pos+1) == '>'){
                *pos++ = '\0';
                *pos = '\0';
                pos++;
                while(isspace(*pos)) pos++;
                redirfilename = pos;
                redir = APPEND_RDIR;
            }
            else{
                *pos = '\0';
                pos++;
                while(isspace(*pos)) pos++;
                redirfilename = pos;
                redir = IN_RDIR;
            }
        }
        else if(*pos == '<'){
            *pos = '\0';
            pos++;
            // 跳过空格
            while(isspace(*pos)) pos++;
            redirfilename = pos;
            redir = IN_RDIR;
        }
        else{

        }
        pos++;
    }
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

    CheckRedir(commandline);

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
    printf("id:%d", id);
    if(id < 0){
        perror("fork error");
        return;
    }
    else if(id == 0){
        printf("child");
        int fd = 0;
        if(redir == IN_RDIR){
            fd = open(redirfilename, O_RDONLY);
            dup2(fd, 0);
        }
        else if(redir == OUT_PDIR){
            fd = open(redirfilename, O_CREAT|O_TRUNC|O_WRONLY);
            dup2(fd, 1);
        }
        else if(redir == APPEND_RDIR){
            fd = open(redirfilename, O_CREAT|O_APPEND|O_WRONLY);
            dup2(fd, 1);
        }

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



// 内建命令由自己完成，不是进程
int BuildCommand(char* argv[], int& argc)
{
    if(argc == 2 && strcmp(argv[0], "cd") == 0){
        chdir(argv[1]);
        GetPWD();
        sprintf(getenv("PWD"), "%s", pwd);
        return 1;
    }
    else if(argc == 2 && strcmp(argv[0], "export") == 0){
        // putenv是将指针指向argv而不是拷贝，所以这里需要先拷贝
        strcpy(myenv, argv[1]);
        putenv(myenv);
        return 1;
    }
    else if(argc == 2 && strcmp(argv[0], "echo") == 0)
    {
        if(*argv[1] == '$'){
            if(*(argv[1]+1) == '?') {
                printf("%d\n", lastCode);
                return 1;
            }
            char* env = getenv(argv[1]+1);
            if(env) printf("%s\n", env);
            else printf("\n");
        }
        else{
            printf("%s\n", argv[1]);
        }
        return 1;
    }
    if(strcmp(argv[0], "ls") == 0){
        argv[argc++] = "--color";
        argv[argc] = NULL;
        return 0;
    }
    
    return 0;
    
}


int main()
{
    char commandline[LINE_SIZE];
    char *argv[ARGC_SIZE];

    int quit = 0;
    while(!quit){
        redirfilename = nullptr;
        redir = NONE;

        // 命令行交互        
        interaction(commandline, LINE_SIZE);

        // 分割命令
        int argc = splitString(commandline, argv);
        if(argc == 0) continue;

        // 内建命令
        lastCode = BuildCommand(argv, argc);
        

        // 命令执行
        if(lastCode != 1) NormalExcute(argv);
    }   
        
    return 0;
}