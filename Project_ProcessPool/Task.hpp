
#include <string>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;


class Task{
public:
    Task()
        :_tasknum(5)
    {}

    void task1(){
        cout<<"野怪刷新"<<endl;
    }

    void task2(){
        cout<<"小兵刷新"<<endl;

    } 

    void task3(){
        cout<<"释放技能"<<endl;

    }

    void task4(){
        cout<<"对局开始"<<endl;

    }

    void task5(){
        cout<<"对局结束"<<endl;

    }

    void operator[](int i)
    {     
        cout<<i<<"---";   
        switch(i){
            case 1: task1(); break;
            case 2: task2(); break;
            case 3: task3(); break;
            case 4: task4(); break;
            case 5: task5(); break;
            default: cout<<"no task" << endl; break;
            
        }
    }
    int _tasknum;
};
