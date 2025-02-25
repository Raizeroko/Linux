#include "Task.hpp"
#include "BlockQueue.hpp"
#include <string>
#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

#define CONSUMER_NUM 3
#define PRODUCER_NUM 5


void* Consumer(void* args){
    BlockQueue<Task>* bq = static_cast<BlockQueue<Task>*>(args);
    while(true){
        //获取任务
        Task t = bq->Pop();
        //完成消费
        t.Run();

        std::cout << "处理任务: " << t.GetTask() << " 运算结果是： " << t.GetResult() << " thread id: " << pthread_self() << std::endl;
        fflush(stdout);
        
        sleep(1);
    } 
    return nullptr;

}


void* Producer(void* args){
    
    BlockQueue<Task>* bq = static_cast<BlockQueue<Task>*>(args);
    std::string ops = "+-*/%";
    
    while(true){
        char op = ops[rand()%ops.size()];
        int num1 = rand()%10;
        int num2 = rand()%10;   
        //创造任务
        Task t(num1, op, num2);
        //放入队列等待消费
        bq->Push(t);

        std::cout << "生产了一个任务: " << t.GetTask() << " thread id: " << pthread_self() << std::endl;
        fflush(stdout);
        sleep(1);
    }
    
    return nullptr;
}


int main(){
    pthread_t consumers[CONSUMER_NUM];
    pthread_t procuders[PRODUCER_NUM];

    srand(time(nullptr)^5);
    BlockQueue<Task>* bq = new BlockQueue<Task>();

    for(int i = 0; i < PRODUCER_NUM; i++){
        pthread_create(&(procuders[i]), nullptr, Producer, bq);
    }
    for(int i = 0; i < PRODUCER_NUM; i++){
        pthread_create(&(consumers[i]), nullptr, Consumer, bq);
    }

    for(int i = 0; i < PRODUCER_NUM; i++){
        pthread_join(procuders[i], nullptr);
    }
    for(int i = 0; i < PRODUCER_NUM; i++){
        pthread_join(consumers[i], nullptr);
    }



}


