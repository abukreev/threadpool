#include <threadpool.h>
#include <unistd.h>

void fun() {

    static int counter = 0;
    int value = ++counter;

    std::cerr << "Task #" << value << " started" << std::endl;
    sleep(1);
    std::cerr << "Task #" << value << " finished" << std::endl;
}

int main () {

    ThreadPool pool(2);

    sleep(1);

    for (int i = 0; i < 10; i++) {
        pool.enqueueJob(fun);
    }

    pool.wait();
    sleep(1);

    for (int i = 0; i < 10; i++) {
        pool.enqueueJob(fun);
    }

    pool.wait();
    pool.drain();

    return 0;
}
