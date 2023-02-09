#include "stdafx.h"
#include "CommandThread.h"

//--------------------------------------------------------------------------------------------------

struct ThreadSignal
{
	ThreadSignal(ESignalId id, std::shared_ptr<void> data)
		: Id(id)
		, signalData(data)
	{}

	ESignalId Id;
	std::shared_ptr<void> signalData;
};

//--------------------------------------------------------------------------------------------------

CommandThread::CommandThread(const char* threadName)
	: m_thread(nullptr)
	, m_timerExit(false)
	, m_pName(threadName)
{
}



CommandThread::~CommandThread()
{
	ExitThread();
}



std::thread::id CommandThread::GetCurrentThreadId()
{
	return std::this_thread::get_id();
}

//--------------------------------------------------------------------------------------------------

bool CommandThread::CreateThread()
{
	if (!m_thread) {
		m_thread = std::make_unique<std::thread>(&CommandThread::Process, this);
	}

	return true;
}



std::thread::id CommandThread::GetThreadId()
{
	DEBUG_VALID(m_thread);
	return m_thread->get_id();
}



void CommandThread::ExitThread()
{
	if (m_thread == nullptr) {
		return;
	}

	// Create a new ThreadSignal
	auto signal = std::make_shared<ThreadSignal>(ESignalId::ThreadExit, nullptr);
	// Put exit thread message into the queue
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_queue.push(signal);
		m_condition.notify_one();
	}

	m_thread->join();
	m_thread = nullptr;
}

// Add signal to queue and notify worker thread
#define PushSignalToQueue(signal) \
std::unique_lock<std::mutex> lock(m_mutex); \
m_queue.push(signal); \
m_condition.notify_one();

void CommandThread::PostSignal(std::shared_ptr<UserData> data)
{
	DEBUG_VALID(m_thread);

	// Create a new ThreadSignal on the heap
	auto signal = std::make_shared<ThreadSignal>(ESignalId::UserData, data);
	PushSignalToQueue(signal);
}

void CommandThread::PostSignal(ESignalId id)
{
	DEBUG_VALID(m_thread);

	auto signal = std::make_shared<ThreadSignal>(id, nullptr);
	PushSignalToQueue(signal);
}

void CommandThread::PostSignal(const wchar_t* pContent)
{
	DEBUG_VALID(m_thread);

	auto signal = std::make_shared<ThreadSignal>(
		ESignalId::UserData,
		std::make_shared<UserData>(pContent)
	);
	PushSignalToQueue(signal);
}



void CommandThread::TimerThread()
{
	//:WARNING - 250ms?
	using namespace std;

	while (m_timerExit == false) {
		// Sleep for 250ms then put a MSG_TIMER into the message queue
		std::this_thread::sleep_for(250ms);
		// Add timer signal to queue and notify worker thread
		PostSignal(ESignalId::ThreadTimer);
	}
}

#undef PushSignalToQueue

void CommandThread::Process()
{
	m_timerExit = false;
	//:TEST - start dummy timer
	//std::thread timerThread(&CommandThread::TimerThread, this);

	while (true) {
		// Wait for a message to be added to the queue
		std::unique_lock<std::mutex> lock(m_mutex);
		while (m_queue.empty()) {
			m_condition.wait(lock);
		}

		if (m_queue.empty()) {
			continue;
		}

		auto signal = m_queue.front();
		m_queue.pop();

		switch (signal->Id) {
		case ESignalId::UserData: {
			DEBUG_VALID(signal->signalData);
			auto userData = std::static_pointer_cast<UserData>(signal->signalData);
			OnUserData(userData);
		} break;

		case ESignalId::Repaint: {
			OnRepaint();
		} break;

		case ESignalId::ThreadTimer:
			// Timer fired
			break;

		case ESignalId::ThreadExit: {
			// Stop timer
			//m_timerExit = true;
			//timerThread.join();
			return;
		}

		default:
			DEBUG_STOP;
			break;
		}
	}
}

