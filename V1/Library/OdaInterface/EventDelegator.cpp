#include "stdafx.h"

#include "EventDelegator.h"

#include <mutex>

//**************************************************************************************************

bool EventDelegator::OnSignal(std::shared_ptr<EventWrapper> wrapper)
{
#define OnAction(x) SgnView::Action::On##x: On##x(pSignal); break

	WorkerThread::Event e = (WorkerThread::Event)wrapper->Type;

	if (e == WorkerThread::Event::Signal) {
		SignalParams* pSignal = (SignalParams*)wrapper->EventData;
		if (pSignal == nullptr) {
			RETURN_FALSE;
		}

		if (pSignal->Target == Sgn::ETarget::View) {
			switch ((SgnView::Action)pSignal->Action) {
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

//--------------------------------------------------------------------------------------------------

EventDelegator::EventDelegator()
	: WorkerThread()
{
}



EventDelegator::~EventDelegator()
{
}



bool EventDelegator::PostSignal(int type, int id, SignalParams* pSignal)
{
	DEBUG_VALID(pSignal);
	DEBUG_VALID(ThreadPtr);

	auto wrapper = std::make_shared<EventWrapper>(type, id, pSignal);
	// Add signal to queue and notify worker thread
	std::unique_lock<std::mutex> lock(ThreadMutex);
	SignalQueue.push(wrapper);
	ThreadCondition.notify_one();

	return true;
}

bool EventDelegator::PostSignal(SignalParams* pSignal)
{
	return PostSignal((int)WorkerThread::Event::Signal, pSignal->ViewId, pSignal);
}



bool EventDelegator::PushSignal(SignalParams* pSignal)
{
	auto wrapper = std::make_shared<EventWrapper>((int)WorkerThread::Event::Signal, pSignal->ViewId, pSignal);
	SignalQueue.push(wrapper);

	return true;
}



bool EventDelegator::SendSignal(SignalParams* pSignal)
{
	// WARNING - check validation
	auto wrapper = std::make_shared<EventWrapper>((int)WorkerThread::Event::Signal, pSignal->ViewId, pSignal);
	return OnSignal(wrapper);
}
