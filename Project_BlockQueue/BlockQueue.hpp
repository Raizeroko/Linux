#include <queue>
#include <pthread.h>
#include <semaphore.h>
#define DEFAULT_SIZE 10

template<class T>
class BlockQueue{

public:
    BlockQueue(int queue_size = DEFAULT_SIZE)
    :_queue_size(queue_size)
    {
        // sem_init(&_cons_sem, 0, 0);
        // sem_init(&_prod_sem, 0, _queue_size);
        pthread_cond_init(&_cons_cond, nullptr);
        pthread_cond_init(&_prod_cond, nullptr);

        pthread_mutex_init(&_mutex, nullptr);
    }

    void Push(T task){
        // sem_wait(&_prod_sem);
        pthread_mutex_lock(&_mutex);
        while(_q.size() == _queue_size){
            pthread_cond_wait(&_prod_cond, &_mutex);
        }
        _q.push(task);
        pthread_cond_signal(&_cons_cond);
        pthread_mutex_unlock(&_mutex);
        // sem_post(&_cons_sem);
    }


    T Pop(){
        // sem_wait(&_cons_sem);
        pthread_mutex_lock(&_mutex);
        while(_q.size() == 0){
            pthread_cond_wait(&_cons_cond, &_mutex);
        }
        T task = _q.front();
        _q.pop();
        pthread_cond_signal(&_prod_cond);
        pthread_mutex_unlock(&_mutex);
        // sem_post(&_prod_sem);
        return task;
    }

    ~BlockQueue()
    {
        // sem_destroy(&_cons_sem);
        // sem_destroy(&_prod_sem);

        pthread_cond_destroy(&_cons_cond);
        pthread_cond_destroy(&_prod_cond);

        pthread_mutex_destroy(&_mutex);
    }

private:
    std::queue<T> _q;
    int _queue_size;

    pthread_mutex_t _mutex;
    pthread_cond_t _cons_cond;
    pthread_cond_t _prod_cond;
    // sem_t _cons_sem;
    // sem_t _prod_sem;
};