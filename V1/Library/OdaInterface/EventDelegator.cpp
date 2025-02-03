#include "stdafx.h"
#include "EventDelegator.h"

//**************************************************************************************************

EventDelegator::EventDelegator()
	: WorkerThread()
{
}



EventDelegator::~EventDelegator()
{
}



bool EventDelegator::PostSignal(int type, int id, SignalArgs::Base* pSignal)
{
	DEBUG_VALID(pSignal);
	DEBUG_VALID(m_thread);

	auto wrapper = std::make_shared<EventWrapper>(type, id, pSignal);
	// Add signal to queue and notify worker thread
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push(wrapper);
	m_condition.notify_one();

	return true;
}

bool EventDelegator::PostSignal(SignalArgs::Base* pSignal)
{
	return PostSignal((int)WorkerThread::Event::Signal, pSignal->ViewId, pSignal);
}



void EventDelegator::PushSignal(SignalArgs::Base* pSignal)
{
	auto wrapper = std::make_shared<EventWrapper>((int)WorkerThread::Event::Signal, pSignal->ViewId, pSignal);
	m_queue.push(wrapper);
}



void EventDelegator::SendSignal(SignalArgs::Base* pSignal)
{
	// WARNING - check validation
	auto wrapper = std::make_shared<EventWrapper>((int)WorkerThread::Event::Signal, pSignal->ViewId, pSignal);
	OnSignal(wrapper);
}



bool EventDelegator::OnSignal(std::shared_ptr<EventWrapper> wrapper)
{
#define OnAction(x) Signal::View::Action::On##x: On##x(pSignal); break

	WorkerThread::Event e = (WorkerThread::Event)wrapper->Type;

	if (e == WorkerThread::Event::Signal) {
		SignalArgs::Base* pSignal = (SignalArgs::Base*)wrapper->EventData;
		if (pSignal == nullptr) {
			RETURN_FALSE;
		}

		if (pSignal->Target == Signal::Target::View) {
			switch ((Signal::View::Action)pSignal->Action) {
			case OnAction(Command);
			case OnAction(ContextCommand);
			case OnAction(KeyDown);
			case OnAction(Initialize);
			case OnAction(LButtonDown);
			case OnAction(LButtonUp);
			case OnAction(MButtonDown);
			case OnAction(MButtonUp);
			case OnAction(RButtonDown);
			case OnAction(RButtonUp);
			case OnAction(MouseMove);
			case OnAction(MouseWheel);
			case OnAction(Paint);
			case OnAction(Resize);
			case OnAction(Input);

			default:
				RETURN_FALSE;
			}
		}
		else {
			RETURN_FALSE;
		}
	}
	else if (e == WorkerThread::Event::Close) {
		return OnClose();
	}
	else {
		RETURN_FALSE;
	}

	return true;

#undef OnAction
}
