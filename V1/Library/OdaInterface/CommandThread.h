#pragma once

#include "Common_Define.h"
#include "Signal.h"

#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <thread>

//--------------------------------------------------------------------------------------------------

struct CommandData
{
};

struct ThreadSignal;

class OdEdCommandContext;
typedef void (*CommandFunction)(OdEdCommandContext* pContext);

//:REF - https://www.codeproject.com/Articles/1169105/Cplusplus-std-thread-Event-Loop-with-Message-Queue

class CommandThread
{
public:

    CommandThread(const char* threadName);

    virtual ~CommandThread();
    // Get the ID of the currently executing thread
    static std::thread::id GetCurrentThreadId();

protected:

    // For posting child signal
    virtual void OnRepaint() {
        DEBUG_STOP;
    }
    // For connector signal
    virtual void OnCommandData(std::shared_ptr<CommandData>) {
        DEBUG_STOP;
    }

public:

    // Called once to create the worker thread
    bool CreateThread();
    // Called once a program exit to exit the worker thread
    void ExitThread();
    // Get the ID of this thread instance
    std::thread::id GetThreadId();
    // Add a signal to the thread queue
    void PostSignal(std::shared_ptr<CommandData> data);
    // Post single signal - PostSignal(ESignalId::Repaint)
    void PostSignal(ESignalId id);
    // Post user data
    void PostSignal(const wchar_t* pContent);

private:

    //:WARNING - disabed
    CommandThread(const CommandThread&) = delete;
    CommandThread& operator =(const CommandThread&) = delete;
    // Entry point for the worker thread
    void Process();
    // Entry point for main(single) timer thread
    void TimerThread();

private:

    CommandFunction* m_pfCommandFunction;
    // main thread
    std::unique_ptr<std::thread> m_thread;
    // main signal queue
    std::queue<std::shared_ptr<ThreadSignal>> m_queue;
    // class for mutual exclusion
    std::mutex m_mutex;
    // class for waiting for conditions
    std::condition_variable m_condition;
    // atomic value
    std::atomic<bool> m_timerExit;
    // thread name
    const char* m_pName;
};
