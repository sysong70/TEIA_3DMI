#pragma once

#include "Common_Define.h"

#include <map>

class WorkerThread;



class ThreadPool
{
public:

    ThreadPool();

    ~ThreadPool();

public:

    void AddNew(int id, WorkerThread* thread);

    void Clear();

    WorkerThread* Find(int id);

    void Remove(int id);

protected:

    std::map<int, WorkerThread*> m_pool;
};
