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
	WorkerThread::Event e = (WorkerThread::Event)wrapper->Type;

	if (e == WorkerThread::Event::Signal) {
		SignalArgs::Base* pSignal = (SignalArgs::Base*)wrapper->EventData;
		if (pSignal == nullptr) {
			RETURN_FALSE;
		}

		if (pSignal->Target == Signal::Target::View) {
			//TRACE(L"%s", (LPCTSTR)pSignal->Dump());

			switch ((Signal::View::Action)pSignal->Action) {
			case Signal::View::Action::OnCommand:     OnCommand((SignalArgs::Command*)pSignal);       break;
			case Signal::View::Action::OnInitialize:  OnInitialize((SignalArgs::Initialize*)pSignal); break;
			case Signal::View::Action::OnLButtonDown: OnLButtonDown((SignalArgs::Mouse*)pSignal);     break;
			case Signal::View::Action::OnLButtonUp:   OnLButtonUp((SignalArgs::Mouse*)pSignal);       break;
			case Signal::View::Action::OnMButtonDown: OnMButtonDown((SignalArgs::Mouse*)pSignal);     break;
			case Signal::View::Action::OnMButtonUp:   OnMButtonUp((SignalArgs::Mouse*)pSignal);       break;
			case Signal::View::Action::OnRButtonDown: OnRButtonDown((SignalArgs::Mouse*)pSignal);     break;
			case Signal::View::Action::OnRButtonUp:   OnRButtonUp((SignalArgs::Mouse*)pSignal);       break;
			case Signal::View::Action::OnMouseMove:   OnMouseMove((SignalArgs::Mouse*)pSignal);       break;
			case Signal::View::Action::OnMouseWheel:  OnMouseWheel((SignalArgs::Mouse*)pSignal);      break;
			case Signal::View::Action::OnPaint:       OnPaint((SignalArgs::Paint*)pSignal);           break;
			case Signal::View::Action::OnResize:      OnResize((SignalArgs::Resize*)pSignal);         break;
			case Signal::View::Action::OnText:        OnText((SignalArgs::Text*)pSignal);             break;

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
}
