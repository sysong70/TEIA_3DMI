#include "stdafx.h"
#include "EventDelegator.h"



EventDelegator::EventDelegator()
	: WorkerThread()
{
}



EventDelegator::~EventDelegator()
{
}



void EventDelegator::PostSignal(int type, int id, SignalArgs::Base* signal)
{
	DEBUG_VALID(signal);
	DEBUG_VALID(m_thread);

	auto wrapper = std::make_shared<EventWrapper>(type, id, signal);
	// Add signal to queue and notify worker thread
	std::unique_lock<std::mutex> lock(m_mutex);
	m_queue.push(wrapper);
	m_condition.notify_one();
}

void EventDelegator::PostSignal(SignalArgs::Base* signal)
{
	PostSignal((int)WorkerThread::Event::Signal, signal->ViewId, signal);
}

void EventDelegator::PushSignal(SignalArgs::Base* signal)
{
	auto wrapper = std::make_shared<EventWrapper>((int)WorkerThread::Event::Signal, signal->ViewId, signal);
	m_queue.push(wrapper);
}

void EventDelegator::SendSignal(SignalArgs::Base* signal)
{
	//:WARNING - check validation
	auto wrapper = std::make_shared<EventWrapper>((int)WorkerThread::Event::Signal, signal->ViewId, signal);
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
		//TRACE(L"%s", (LPCTSTR)pSignal->Dump());

		if (pSignal->Target == Signal::Target::View) {
			switch ((Signal::View::Action)pSignal->Action) {
				//case Signal::View::Action::OnCommand:     OnCommand(pSignal);     break;
				//case Signal::View::Action::OnInitialize:	OnInitialize(pSignal);	break;
				//case Signal::View::Action::OnLButtonDown:	OnLButtonDown(pSignal);	break;
				//case Signal::View::Action::OnLButtonUp:		OnLButtonUp(pSignal);	break;
				//case Signal::View::Action::OnMButtonDown:	OnMButtonDown(pSignal);	break;
				//case Signal::View::Action::OnMButtonUp:		OnMButtonUp(pSignal);	break;
				//case Signal::View::Action::OnRButtonDown:	OnRButtonDown(pSignal);	break;
				//case Signal::View::Action::OnRButtonUp:		OnRButtonUp(pSignal);	break;
				//case Signal::View::Action::OnMouseMove:		OnMouseMove(pSignal);	break;
				//case Signal::View::Action::OnMouseWheel:	OnMouseWheel(pSignal);	break;
				//case Signal::View::Action::OnPaint:			OnPaint(pSignal);		break;
				//case Signal::View::Action::OnResize:		OnResize(pSignal);		break;
				//case Signal::View::Action::OnInput:			OnInput(pSignal);		break;

			case OnAction(Command);
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

			case Signal::View::Action::OnConstruct:
			case Signal::View::Action::OnDestruct:
				return true;

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
