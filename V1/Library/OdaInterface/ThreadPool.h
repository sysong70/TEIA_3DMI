#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

//--------------------------------------------------------------------------------------------------

class ThreadPool
{
    std::vector<std::thread> Workers;
    std::queue<std::function<void()>> Tasks;
    std::mutex Mutex;
    std::condition_variable Condiation;
    bool Working = true;

public:

    //:CHECK - maximum count
    ThreadPool(size_t threads = 4);

    ~ThreadPool();

    template<class F> void Enqueue(F&& f);
};



template<class F>
void ThreadPool::Enqueue(F&& f)
{
    //:WARNING - auto unlock
    {
        std::unique_lock<std::mutex> lock(Mutex);
        Tasks.emplace(std::forward<F>(f));
    }
    Condiation.notify_one();
}
