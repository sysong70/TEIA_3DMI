#include "stdafx.h"
#include "WorkerThread.h"
#include "Json.h"

#pragma warning(disable : 4996)

//**************************************************************************************************

#pragma region EventWrapper Structure

EventWrapper::EventWrapper(int type, int id, void* pEventData, void* pArrayData)
	: Type(type)
	, Id(id)
{
	if (pEventData != nullptr) {
		EventData = pEventData;
	}
	
	if (pArrayData != nullptr) {
		EventData = pArrayData;
		IsArray = true;
	}
}



EventWrapper::~EventWrapper()
{
	if (IsArray) {
		REMOVE_ARRAY(EventData);
	}
	else {
		REMOVE_POINTER(EventData);
	}
}

#pragma endregion //:REGION

//**************************************************************************************************

WorkerThread::WorkerThread()
{
}



WorkerThread::~WorkerThread()
{
	TerminateThread();
}



std::thread::id WorkerThread::GetCurrentThreadId()
{
	return std::this_thread::get_id();
}



bool WorkerThread::CreateThread()
{
	if (!m_thread) {
		m_thread = std::make_unique<std::thread>(&WorkerThread::Process, this);
		return true;
	}
	else {
		RETURN_FALSE;
	}
}



void WorkerThread::TerminateThread()
{
	if (m_thread == nullptr) {
		return;
	}

	auto wrapper = std::make_shared<EventWrapper>((int)Event::Close);
	std::unique_lock<std::mutex> lock(m_mutex);
	// Put close event into the queue
	m_queue.push(wrapper);
	m_condition.notify_one();
	//:ERROR - End thread, dead lock!
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

	// Add evnet to queue and notify worker thread
	auto event = std::make_shared<EventWrapper>((int)type, id, pEventData);
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push(event);
	m_condition.notify_one();
}

void WorkerThread::PostEvent(const wchar_t* pEventData, bool copyData)
{
	if (m_thread == nullptr) {
		DEBUG_RETURN;
	}

	wchar_t* pData = (wchar_t*)pEventData;
	if (copyData) {
		int length = ::wcslen(pEventData);
		pData = new wchar_t[length + 1];
		::wcsncpy(pData, pEventData, length);
		pData[length] = 0;
	}

	// Add evnet to queue and notify worker thread
	auto event = std::make_shared<EventWrapper>((int)Event::Signal, 0, nullptr, (void*)pData);
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push(event);
	m_condition.notify_one();
}



void WorkerThread::SetSignalFunc(std::function<void(const wchar_t*)> func)
{
	m_pSignalFunc = func;
}



void WorkerThread::SetTimerFunc(std::function<void()> func)
{
	m_pTimerFunc = func;
}



void WorkerThread::SetUserFunc(std::function<void(const wchar_t*)> func)
{
	m_pUserFunc = func;
}



void WorkerThread::StartTimer(UINT milliseconds)
{
	if (milliseconds > 0 && m_timer == nullptr) {
		m_timerInterver = milliseconds;
		m_timer = std::make_unique<std::thread>(&WorkerThread::TimerThread, this);
	}
}

//--------------------------------------------------------------------------------------------------

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

		case Event::Timer:	OnTimer(); break;
		case Event::Signal:	OnSignal(wrapper); break;
		case Event::User:	OnUser(wrapper); break;

		default:
			DEBUG_STOP;
			break;
		}
	}
}



bool WorkerThread::OnTimer()
{
	if (m_pTimerFunc != nullptr) {
		m_pTimerFunc();
		return true;
	}

	return false;
}



bool WorkerThread::OnSignal(std::shared_ptr<EventWrapper> wrapper)
{
	if (m_pSignalFunc != nullptr) {
		m_pSignalFunc((const wchar_t*)wrapper->EventData);
		return true;
	}

	OutputDebugString(L"WorkerThread::OnSignal\n");
	return false;
}



bool WorkerThread::OnUser(std::shared_ptr<EventWrapper> wrapper)
{
	if (m_pUserFunc != nullptr) {
		m_pUserFunc((const wchar_t*)wrapper->EventData);
		return true;
	}

	OutputDebugString(L"WorkerThread::OnUser\n");
	return false;
}

//--------------------------------------------------------------------------------------------------

void WorkerThread::TimerThread()
{
	while (m_timerExit == false) {
		std::this_thread::sleep_for(std::chrono::milliseconds(m_timerInterver));
		PostEvent(Event::Timer);
	}
}
