#include "stdafx.h"

#include "ThreadPool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

ThreadPool::ThreadPool(size_t threads)
{
    for (size_t i = 0; i < threads; ++i) {
        Workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                //:WARNING - auto unlock
                {
                    std::unique_lock<std::mutex> lock(this->Mutex);
                    this->Condiation.wait(lock, [this] {
                        return this->Working == false || this->Tasks.empty() == false;
                    });

                    if (this->Working == false && this->Tasks.empty()) {
                        return;
                    }

                    task = std::move(this->Tasks.front());
                    this->Tasks.pop();
                }

                task();
            }
        });
    }
}



ThreadPool::~ThreadPool()
{
    //:WARNING - auto unlock
    {
        std::unique_lock<std::mutex> lock(Mutex);
        Working = false;
    }

    Condiation.notify_all();
    for (std::thread& worker : Workers) {
        worker.join();
    }
}
