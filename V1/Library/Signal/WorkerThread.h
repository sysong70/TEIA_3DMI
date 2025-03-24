#pragma once

#include "Common_Define.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

//--------------------------------------------------------------------------------------------------
// REF - https://www.codeproject.com/Articles/1169105/Cplusplus-std-thread-Event-Loop-with-Message-Queue

struct EventWrapper
{
    int Type;                   // WorkerThread::Event
    int Id = -1;                // Any id
    void* EventData = nullptr;  // Data buffer
    bool IsArray = false;       // Is EventData array?

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

protected: // WARNING - disabed

    WorkerThread(const WorkerThread&) = delete;
    WorkerThread& operator =(const WorkerThread&) = delete;

public:

    // Called once to create the worker thread
    bool Create();
    // Called once to exit the worker thread
    void Terminate();
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
    void MainProcess();

protected: // Process switch

    // Single timer
    virtual bool OnTimer();

    virtual bool OnSignal(std::shared_ptr<EventWrapper> wrapper);

    virtual bool OnUser(std::shared_ptr<EventWrapper> wrapper);

protected:

    // main thread
    std::unique_ptr<std::thread> ThreadPtr = nullptr;
    // main signal queue
    std::queue<std::shared_ptr<EventWrapper> > SignalQueue;
    // class for mutual exclusion
    std::mutex ThreadMutex;
    // class for waiting for conditions
    std::condition_variable ThreadCondition;
    // signal process function
    std::function<void(const wchar_t*)> SignalFunc = nullptr;
    // user process function
    std::function<void(const wchar_t*)> UserFunc = nullptr;

protected: // Single Timer

    // thread safe variable
    std::atomic<int> TimerInterver;
    // thread safe variable
    std::atomic<bool> TimerExit;
    // time thread
    std::unique_ptr<std::thread> TimerPtr;
    // timer process function
    std::function<void()> TimerFunc = nullptr;

    void TimerProcess();
};
