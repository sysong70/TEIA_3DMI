#include "stdafx.h"

#include "WorkerThread.h"

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

#pragma endregion // REGION

//**************************************************************************************************

WorkerThread::WorkerThread()
{
	//Create();
}



WorkerThread::~WorkerThread()
{
	//Terminate();
}



std::thread::id WorkerThread::GetCurrentThreadId()
{
	return std::this_thread::get_id();
}



bool WorkerThread::Create()
{
	if (!ThreadPtr) {
		ThreadPtr = std::make_unique<std::thread>(&WorkerThread::MainProcess, this);
		//ThreadPtr->detach();
		return true;
	}
	else {
		RETURN_FALSE;
	}
}



void WorkerThread::Terminate()
{
	if (ThreadPtr == nullptr) {
		DEBUG_RETURN;
	}

	// Put close event into the queue
	auto event = std::make_shared<EventWrapper>((int)Event::Close);
	// WARNING - do not lock
	//std::unique_lock<std::mutex> lock(ThreadMutex);
	SignalQueue.push(event);
	ThreadCondition.notify_one();

	if (ThreadPtr->joinable()) {
		ThreadPtr->join();
	}
}



std::thread::id WorkerThread::GetThreadId()
{
	DEBUG_VALID(ThreadPtr);
	return ThreadPtr->get_id();
}



void WorkerThread::PostEvent(Event type, int id, void* pEventData)
{
	if (ThreadPtr == nullptr) {
		DEBUG_RETURN;
	}

	// Add evnet to queue and notify worker thread
	auto event = std::make_shared<EventWrapper>((int)type, id, pEventData);
	std::unique_lock<std::mutex> lock(ThreadMutex);
	SignalQueue.push(event);
	ThreadCondition.notify_one();
}

void WorkerThread::PostEvent(const wchar_t* pEventData, bool copyData)
{
	if (ThreadPtr == nullptr) {
		DEBUG_RETURN;
	}

	wchar_t* pData = (wchar_t*)pEventData;
	if (copyData) {
		size_t length = ::wcslen(pEventData);
		pData = new wchar_t[length + 1];
		::wcsncpy(pData, pEventData, length);
		pData[length] = 0;
	}

	// Add evnet to queue and notify worker thread
	auto event = std::make_shared<EventWrapper>((int)Event::Signal, 0, nullptr, (void*)pData);
	std::unique_lock<std::mutex> lock(ThreadMutex);
	SignalQueue.push(event);
	ThreadCondition.notify_one();
}



void WorkerThread::SetSignalFunc(std::function<void(const wchar_t*)> func)
{
	SignalFunc = func;
}



void WorkerThread::SetTimerFunc(std::function<void()> func)
{
	TimerFunc = func;
}



void WorkerThread::SetUserFunc(std::function<void(const wchar_t*)> func)
{
	UserFunc = func;
}



void WorkerThread::StartTimer(UINT milliseconds)
{
	//if (milliseconds > 0 && TimerPtr == nullptr) {
	//	TimerInterver = milliseconds;
	//	TimerPtr = std::make_unique<std::thread>(&WorkerThread::TimerProcess, this);
	//}
}

//--------------------------------------------------------------------------------------------------

void WorkerThread::MainProcess()
{
	while (true) {
		// Wait for a message to be added to the queue
		std::unique_lock<std::mutex> lock(ThreadMutex);
		while (SignalQueue.empty()) {
			ThreadCondition.wait(lock);
		}

		if (SignalQueue.empty()) {
			continue;
		}

		auto wrapper = SignalQueue.front();
		SignalQueue.pop();

		switch ((Event)wrapper->Type) {
		case Event::Close:
			if (TimerPtr) {
				TimerExit = true;
				TimerPtr->join();
			}
			return;

		case Event::Timer:
			OnTimer();
			break;

		case Event::Signal:
			if (OnSignal(wrapper) == false) {
				return;
			}
			break;

		case Event::User:
			OnUser(wrapper);
			break;

		default:
			DEBUG_STOP;
			break;
		}
	}
}



bool WorkerThread::OnTimer()
{
	if (TimerFunc != nullptr) {
		TimerFunc();
		return true;
	}

	return false;
}



bool WorkerThread::OnSignal(std::shared_ptr<EventWrapper> wrapper)
{
	if (SignalFunc != nullptr) {
		SignalFunc((const wchar_t*)wrapper->EventData);
		return true;
	}

	OutputDebugString(L"WorkerThread::OnSignal\n");
	return false;
}



bool WorkerThread::OnUser(std::shared_ptr<EventWrapper> wrapper)
{
	if (UserFunc != nullptr) {
		UserFunc((const wchar_t*)wrapper->EventData);
		return true;
	}

	OutputDebugString(L"WorkerThread::OnUser\n");
	return false;
}

//--------------------------------------------------------------------------------------------------

void WorkerThread::TimerProcess()
{
	while (TimerExit == false) {
		std::this_thread::sleep_for(std::chrono::milliseconds(TimerInterver));
		PostEvent(Event::Timer);
	}
}
