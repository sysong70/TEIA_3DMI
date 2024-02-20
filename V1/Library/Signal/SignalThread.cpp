#include "stdafx.h"
#include "WorkerThread.h"
#include "Json.h"



#pragma region EventWrapper Structure

EventWrapper::EventWrapper(int type, int id, void* pEventData)
	: Type(type)
	, Id(id)
	, EventData(pEventData)
{
}



EventWrapper::~EventWrapper()
{
	REMOVE_POINTER(EventData);
}

#pragma endregion //:REGION



WorkerThread::WorkerThread(bool create)
	: m_thread(nullptr)
{
	if (create) {
		Initialize();
	}
}



WorkerThread::~WorkerThread()
{
	Exit();
}



std::thread::id WorkerThread::GetCurrentThreadId()
{
	return std::this_thread::get_id();
}



bool WorkerThread::Initialize()
{
	if (!m_thread) {
		m_thread = std::make_unique<std::thread>(&WorkerThread::Process, this);
		return true;
	}
	else {
		RETURN_FALSE;
	}
}



void WorkerThread::Exit()
{
	if (m_thread == nullptr) {
		return;
	}

	// Create a new event
	auto wrapper = std::make_shared<EventWrapper>((int)Event::Close);
	//:WARNING - do not lock
	//std::lock_guard<std::mutex> lock(m_mutex);

	// Put close event into the queue
	m_queue.push(wrapper);
	m_condition.notify_one();
	// End thread
	m_thread->join();
	m_thread = nullptr;
}



std::thread::id WorkerThread::GetThreadId()
{
	DEBUG_VALID(m_thread);
	return m_thread->get_id();
}



void WorkerThread::PostEvent(Event type, int id, void* pEventData)
{
	if (m_thread == nullptr) {
		DEBUG_RETURN;
	}

	auto event = std::make_shared<EventWrapper>((int)type, id, nullptr);
	// Add evnet to queue and notify worker thread
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push(event);
	m_condition.notify_one();
}



void WorkerThread::SetTimerProcess(std::function<void()> func)
{
	m_pTimerProcess = func;
}



void WorkerThread::StartTimer(UINT milliseconds)
{
	if (milliseconds > 0 && m_timer == nullptr) {
		m_timerInterver = milliseconds;
		m_timer = std::make_unique<std::thread>(&WorkerThread::TimerThread, this);
	}
}



void WorkerThread::Process()
{
	while (true) {
		// Wait for a message to be added to the queue
		std::unique_lock<std::mutex> lock(m_mutex);
		while (m_queue.empty()) {
			m_condition.wait(lock);
		}

		if (m_queue.empty()) {
			continue;
		}

		auto wrapper = m_queue.front();
		m_queue.pop();

		switch ((Event)wrapper->Type) {
		case Event::Close:
			if (m_timer) {
				m_timerExit = true;
				m_timer->join();
			}
			return;

		case Event::Timer: OnTimer(); break;
		case Event::Signal: OnSignal(wrapper); break;
		case Event::User: OnUser(wrapper); break;

		default:
			DEBUG_STOP;
			break;
		}
	}
}



bool WorkerThread::OnTimer()
{
	if (m_pTimerProcess != nullptr) {
		m_pTimerProcess();
	}

	return true;
}



bool WorkerThread::OnSignal(std::shared_ptr<EventWrapper> wrapper)
{
	OutputDebugString(L"WorkerThread::OnSignal\n");
	return false;
}



bool WorkerThread::OnUser(std::shared_ptr<EventWrapper> wrapper)
{
	OutputDebugString(L"WorkerThread::OnUser\n");
	return false;
}



void WorkerThread::TimerThread()
{
	while (m_timerExit == false) {
		std::this_thread::sleep_for(std::chrono::milliseconds(m_timerInterver));
		PostEvent(Event::Timer);
	}
}
