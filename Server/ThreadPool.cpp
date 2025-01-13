#include "ThreadPool.h"

using namespace std;


ThreadPool::ThreadPool(unsigned int numThreads)
{
    this->stop = false;
    for(unsigned int i = 0; i < numThreads; i++){
        this->workers.emplace_back([this] {
            for(;;) {
                std::unique_lock<std::mutex> lock(this->queueMutex);
                this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty();});
                if(this->stop && this->tasks.empty()) return;
                auto task = std::move(this->tasks.front());
                this->tasks.pop();
                lock.unlock();
                task();
            }
        });
    }
}

ThreadPool::~ThreadPool()
{
    std::unique_lock<std::mutex> lock(this->queueMutex);
    this->stop = true;
    lock.unlock();
    this->condition.notify_all();
    for(std::thread& worker: this->workers)
        worker.join();
}

