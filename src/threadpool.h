#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <vector>
#include <list>
#include <iostream>
#include <unistd.h>
#include <assert.h>

#include <stdio.h>

void* threadfun(void *p);

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

class ThreadPool {
public:
    typedef void(*Job)();

private:
    typedef std::vector<pthread_t> Threads;
    Threads d_threads;
    typedef std::list<Job> Queue;
    Queue d_queue;
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

    static volatile int counter = 0;
    counter++;
    std::cerr << __FILE__ << ":" << __LINE__ << ": counter = " << counter << std::endl;

    ThreadPool* pool = (ThreadPool*) p;

//    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    pthread_mutex_lock(&mutex);
//    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;

    while (pool->d_working) {
//        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        pthread_cond_wait(&cond, &mutex);
//        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;

        if (!pool->d_working) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        while (true) {
            if (pool->d_queue.empty()) {
                pthread_mutex_unlock(&mutex);
                break;
            }
//            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            ThreadPool::Job job = *pool->d_queue.begin();
            pool->d_queue.pop_front();
            pthread_cond_broadcast(&cond);
//            std::cerr << __FILE__ << ":" << __LINE__ << ": d_queue.size() = " << pool->d_queue.size() << std::endl;
            pthread_mutex_unlock(&mutex);

            assert(NULL != job);
            job(); // job

            if (!pool->d_working) {
                break;
            }

            pthread_mutex_lock(&mutex);
        }
    }

    counter--;
    std::cerr << __FILE__ << ":" << __LINE__ << ": counter = " << counter << std::endl;

    return NULL;
}

ThreadPool::ThreadPool(size_t numThreads)
: d_working(true)
 {
    d_threads.reserve(numThreads);
    for (size_t i = 0; i < numThreads; ++i) {
        pthread_t thread;
        if(pthread_create(&thread, NULL, threadfun, this)) {
            std::cerr << "Error creating thread" << std::endl;
        }
        d_threads.push_back(thread);
    }
}

ThreadPool::~ThreadPool() {

    drain();
}

void ThreadPool::enqueueJob(ThreadPool::Job job) {

//    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    pthread_mutex_lock(&mutex);
//    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    d_queue.push_back(job);
//    std::cerr << __FILE__ << ":" << __LINE__ << ": d_queue.size() = " << d_queue.size() << std::endl;
    pthread_cond_signal(&cond);
//    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    pthread_mutex_unlock(&mutex);
//    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;

}

void ThreadPool::wait() {

    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    pthread_mutex_lock(&mutex);
    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    while (true) {
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        if (!d_working) {
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            break;
        }
        if (d_queue.empty()) {
            std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
            break;
        }
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        pthread_cond_wait(&cond, &mutex);
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    }
    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    pthread_mutex_unlock(&mutex);
    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
}

void ThreadPool::drain() {

    pthread_mutex_lock(&mutex);
    d_working = false;
    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    pthread_cond_broadcast(&cond);
    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    pthread_mutex_unlock(&mutex);
    std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
    int counter = 0;
    for (Threads::iterator i = d_threads.begin(), end = d_threads.end();
         i != end; ++i) {
        pthread_t& thread = *i;
        std::cerr << __FILE__ << ":" << __LINE__ << std::endl;
        pthread_join(thread, NULL);
        counter++;
        std::cerr << __FILE__ << ":" << __LINE__ << ": thread " << counter << " stopped" << std::endl;
    }
}

#endif // THREADPOOL_H
