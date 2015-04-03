#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <vector>
#include <list>
#include <iostream>
#include <unistd.h>

void* threadfun(void *p);

class ThreadPool {
public:
    typedef void(*Job)();

private:
    typedef std::vector<pthread_t> Threads;
    Threads d_threads;
    typedef std::list<Job> Queue;
    Queue d_queue;
    pthread_mutex_t d_mutex;
    pthread_cond_t d_cv;
    volatile bool d_working;

public:
    ThreadPool(size_t numThreads);
    ~ThreadPool();
    void enqueueJob(Job job);
    void wait();
    void drain();

    friend void* threadfun(void *p);
};

void* threadfun(void *p) {

    std::cerr << "threadfun started" << std::endl;

    ThreadPool* pool = (ThreadPool*) p;

    while (pool->d_working) {

        pthread_mutex_lock(&pool->d_mutex);
        bool empty = pool->d_queue.empty();
        pthread_mutex_unlock(&pool->d_mutex);

        if (empty) {
            std::cerr << "threadfun sleeping" << std::endl;
//            pthread_cond_wait(&pool->d_cv, &pool->d_mutex);
            sleep(1);
            std::cerr << "threadfun woke up" << std::endl;
        } else {    
            pthread_mutex_lock(&pool->d_mutex);
            pool->d_queue.pop_front();
            std::cerr << "threadfun took one job from the queue" << std::endl;
            pthread_mutex_unlock(&pool->d_mutex);

            std::cerr << "threadfun doing the job" << std::endl;
            sleep(5); // job
            std::cerr << "threadfun finished the job" << std::endl;
        }
    }

    std::cerr << "threadfun finished" << std::endl;

    return NULL;
}

ThreadPool::ThreadPool(size_t numThreads) {

    d_working = true;

    pthread_mutex_lock(&d_mutex);

    d_threads.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        pthread_t thread;
        if(pthread_create(&thread, NULL, threadfun, this)) {
            std::cerr << "Error creating thread" << std::endl;
        }
        d_threads.push_back(thread);
    }

    pthread_mutex_unlock(&d_mutex);
}

ThreadPool::~ThreadPool() {

    drain();
}

void ThreadPool::enqueueJob(ThreadPool::Job job) {

    pthread_mutex_lock(&d_mutex);
    d_queue.push_back(job);
    pthread_mutex_unlock(&d_mutex);
    std::cerr << "job was added to the queue" << std::endl;

//    pthread_cond_signal(&d_cv);
//    std::cerr << "conditional variable signaled" << std::endl;
}

void ThreadPool::wait() {

    while (true) {
        pthread_mutex_lock(&d_mutex);
        bool empty = d_queue.empty();
        pthread_mutex_unlock(&d_mutex);
        if (empty) {
            break;
        }
        sleep(1);
    }
}

void ThreadPool::drain() {

    d_working = false;
    pthread_mutex_lock(&d_mutex);
//    pthread_cond_signal(&d_cv);
    d_queue.push_back(NULL);
    pthread_mutex_unlock(&d_mutex);
    for (Threads::iterator i = d_threads.begin(), end = d_threads.end();
         i != end; ++i) {
        if (pthread_join(*i, NULL)) {
            std::cerr << "Error joining thread" << std::endl;
        }
    }
}

#endif // THREADPOOL_H
