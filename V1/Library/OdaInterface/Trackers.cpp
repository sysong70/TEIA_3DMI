#include "stdafx.h"
#include "Trackers.h"

//**************************************************************************************************

void TrackerStack::SetGsView(OdGsView* pView)
{
	for (auto item : m_buffer) {
		item->SetGsView(pView);
	}
}



void TrackerStack::UnsetGsView(OdGsView* pView)
{
	for (auto item : m_buffer) {
		item->UnsetGsView(pView);
	}
}



void TrackerStack::Clear(OdGsView* pView)
{
	UnsetGsView(pView);
	m_buffer.clear();
}

//--------------------------------------------------------------------------------------------------

void TrackerStack::SetValue(double value)
{
	for (auto item : m_buffer) {
		item->SetValue(value);
	}
}



void TrackerStack::SetValue(int value)
{
	for (auto item : m_buffer) {
		item->SetValue(value);
	}
}



void TrackerStack::SetValue(const OdGePoint3d& value)
{
	for (auto item : m_buffer) {
		item->SetValue(value);
	}
}



void TrackerStack::SetValue(CString value)
{
	for (auto item : m_buffer) {
		item->SetValue(value);
	}
}

//--------------------------------------------------------------------------------------------------

void TrackerStack::Push(TrackerBase* pTracker)
{
	if (pTracker != nullptr) {
		m_buffer.push_back(pTracker);
	}
}



void TrackerStack::Pop(TrackerBase* pTracker)
{
	if (pTracker != nullptr) {
		if (pTracker == m_buffer.back()) {
			m_buffer.pop_back();
			DEBUG_STOP;
		}
	}
}
