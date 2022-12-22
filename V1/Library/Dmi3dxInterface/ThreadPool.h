#pragma once


// https://github.com/bshoshany/thread-pool#motivation

#include <functional>
#include <thread>
#include <queue>
#include <vector>
#include <condition_variable>
#include <mutex>

class ThreadPool
{
public:
	ThreadPool(size_t num_threads);
	~ThreadPool();

	// job 추가
	void EnqueueJob(std::function<void()> job);

private:
	// 총 Worker 쓰레드의 개수
	size_t num_threads;
	// Worker 쓰레드를 보관하는 벡터
	std::vector<std::thread> worker_threads;
	// 할 일들을 보관하는 job 큐
	std::queue<std::function<void()>> jobQue;
	// 위의 job 큐들을 위한 조건 변수와 뮤텍스
	std::condition_variable cv;
	std::mutex mtx;

	// 모든 쓰레드 종료
	bool stop_all;

	// 작업 큐에 있는 작업 하나를 실행한다.
	void WorkerThread();
};
