#pragma once

#include "Common_Define.h"
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <thread>
#include <functional>

//--------------------------------------------------------------------------------------------------
//:REF - https://www.codeproject.com/Articles/1169105/Cplusplus-std-thread-Event-Loop-with-Message-Queue

struct EventWrapper
{
    int Type;                   // WorkerThread::Event
    int Id = -1;                // Any id
    void* EventData = nullptr;  // Data buffer
    bool Array = false;         // Is EventData array?

    EventWrapper(int type, int id = -1, void* pEventData = nullptr, void* pArrayData = nullptr);

    ~EventWrapper();
};

//--------------------------------------------------------------------------------------------------

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

    WorkerThread();

    virtual ~WorkerThread();
    // Get the ID of the currently executing thread
    static std::thread::id GetCurrentThreadId();

protected: //:WARNING - disabed

    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator =(const WorkerThread&) = delete;

public:

    // Called once to create the worker thread
    bool CreateThread();
    // Called once a program exit to exit the worker thread
    void TerminateThread();
    // Get the ID of this thread instance
    std::thread::id GetThreadId();

    void PostEvent(Event type, int id = -1, void* pEventData = nullptr);

    void PostEvent(const wchar_t* pEventData, bool copyData = true);

    void SetSignalFunc(std::function<void(const wchar_t*)> func);

    void SetTimerFunc(std::function<void()> func);

    void SetUserFunc(std::function<void(const wchar_t*)> func);

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
    std::unique_ptr<std::thread> m_thread = nullptr;
    // main signal queue
    std::queue<std::shared_ptr<EventWrapper> > m_queue;
    // class for mutual exclusion
    std::mutex m_mutex;
    // class for waiting for conditions
    std::condition_variable m_condition;
    // signal process function
    std::function<void(const wchar_t*)> m_pSignalFunc = nullptr;
    // user process function
    std::function<void(const wchar_t*)> m_pUserFunc = nullptr;

protected: // Single Timer

    // thread safe variable
    std::atomic<int> m_timerInterver;
    // thread safe variable
    std::atomic<bool> m_timerExit;
    // time thread
    std::unique_ptr<std::thread> m_timer;
    // timer process function
    std::function<void()> m_pTimerFunc = nullptr;

    void TimerThread();
};
