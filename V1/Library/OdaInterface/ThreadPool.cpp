#include "stdafx.h"
#include "ThreadPool.h"
#include "WorkerThread.h"



ThreadPool::ThreadPool()
{
}



ThreadPool::~ThreadPool()
{
}



void ThreadPool::AddNew(int id, WorkerThread* thread)
{
    ASSERT(m_pool.contains(id) == false);
    m_pool[id] = thread;
}



void ThreadPool::Clear()
{
    for (auto thread : m_pool) {
        REMOVE_POINTER(thread.second);
    }
}



WorkerThread* ThreadPool::Find(int id)
{
    if (m_pool.contains(id)) {
        return m_pool[id];
    }
    else {
        RETURN_NULL;
    }
}



void ThreadPool::Remove(int id)
{
    auto thread = m_pool[id];

    m_pool.erase(id);
    REMOVE_POINTER(thread);
}
