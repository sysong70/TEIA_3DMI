#include "stdafx.h"

#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t num_threads)
	:
	num_threads(num_threads),
	stop_all(false)
{
	worker_threads.reserve(num_threads);
	for(size_t i = 0; i < num_threads; ++i)
	{
		// 워커 쓰레드 추가
		worker_threads.emplace_back([this] () { this->WorkerThread(); });
	}
}

// 쓰레드 풀의 종료 상황 - 소멸 시 혹은 임의로 종료하였을 때
// 1. 쓰레드 풀이 종료된 상태라면, 더 이상 작업 요청을 받지 않는다.
// 2. 대기 중인 모든 쓰레드를 깨우고 작업 큐가 비어있다면 워커 쓰레드를 종료시킨다.
// 3. 작업 중 인 워커 쓰레드가 있다면 합류 불가능하게 만들어(join / detach) 계속 처리하도록 한다.
ThreadPool::~ThreadPool()
{
	stop_all = true;
	// 자고 있는 모든 쓰레드를 깨운다.
	cv.notify_all();

	// 합류 불가능하게 만든다.
	for(auto & t : worker_threads)
	{
		t.join();
	}
}

// 작업을 추가하는 함수
void ThreadPool::EnqueueJob(std::function<void()> job)
{
	if(stop_all)
	{
		throw std::runtime_error("ThreadPool 사용 중지 됌");
	}
	{
		std::lock_guard<std::mutex> lock(mtx);

		// 작업 큐 보호하고 작업을 추가한다.
		jobQue.push(std::move(job));
	}
	cv.notify_one();
}

// block 상태에 있다가 작업 요청에 의해 깨어나고 작업 큐에서 작업을 하나 꺼내 수행한다.
void ThreadPool::WorkerThread()
{
	while(true)
	{
		std::unique_lock<std::mutex> lock(mtx);

		// 비어있지 않거나 stop_all일 경우 다음 실행을 진행한다.
		cv.wait(lock, [this] () { return !this->jobQue.empty() || stop_all; });

		// 쓰레드 종료와 모든 작업이 끝낫을 경우 return
		if(stop_all && this->jobQue.empty())
		{
			return;
		}

		// 맨 앞의 작업을 뺀다.
		auto nextJob = std::move(jobQue.front());
		jobQue.pop();
		lock.unlock();

		// 해당 job을 수행한다.
		nextJob();
	}
}