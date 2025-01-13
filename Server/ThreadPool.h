#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool{
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        bool stop;
    public:
        ThreadPool(unsigned int numThreads);
        ~ThreadPool();
        template<class F>
        void enqueue(F&& task);

};


template <class F>
inline void ThreadPool::enqueue(F &&task)
{
    std::unique_lock<std::mutex> lock(this->queueMutex);
    this->tasks.emplace(std::forward<F>(task));
    lock.unlock();
    this->condition.notify_one();
}

#endif


