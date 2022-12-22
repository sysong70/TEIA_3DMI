#pragma once

#include "Common_Define.h"
#include "SignalBase.h"

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <functional>

//:REF - https://www.codeproject.com/Articles/1169105/Cplusplus-std-thread-Event-Loop-with-Message-Queue

class WorkerThread;

struct EventWrapper
{
    int Type;    // WorkerThread::Event
    int Id = -1; // any id
    void* EventData = nullptr;

    EventWrapper(int type, int id = -1, void* pEventData = nullptr);

    ~EventWrapper();
};



class WorkerThread
{
public:

    enum class Event
    {
        Unknown = -1,
        Close,
        Timer,
        Signal,
        User,
    };

    WorkerThread(bool create = true);

    virtual ~WorkerThread();
    // Get the ID of the currently executing thread
    static std::thread::id GetCurrentThreadId();

protected:

    //:WARNING - disabed
    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator =(const WorkerThread&) = delete;

public:

    // Called once to create the worker thread
    bool Initialize();
    // Called once a program exit to exit the worker thread
    void Exit();
    // Get the ID of this thread instance
    std::thread::id GetThreadId();

    void PostEvent(Event type, int id = -1, void* pEventData = nullptr);
    /*
        WorkerThread worker;
        worker.SetTimerProcess(std::bind(&className::method, this));
        worker.SetTimerProcess(&funcName);
    */
    void SetTimerProcess(std::function<void()> func);

    void StartTimer(UINT milliseconds = 0);

protected:

    // Entry point for the worker thread
    void Process();

protected: // Process switch

    // Single timer
    virtual bool OnTimer();

    virtual bool OnSignal(std::shared_ptr<EventWrapper> wrapper);

    virtual bool OnUser(std::shared_ptr<EventWrapper> wrapper);

protected:

    // main thread
    std::unique_ptr<std::thread> m_thread;
    // main signal queue
    std::queue<std::shared_ptr<EventWrapper> > m_queue;
    // class for mutual exclusion
    std::mutex m_mutex;
    // class for waiting for conditions
    std::condition_variable m_condition;

protected: // Single Timer

    std::atomic<int> m_timerInterver;
    std::atomic<bool> m_timerExit;
    std::unique_ptr<std::thread> m_timer;
    std::function<void()> m_pTimerProcess = nullptr;

    void TimerThread();
};
