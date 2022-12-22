#include "stdafx.h"
#include "resource.h"
#include "Component.HistoryBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



Component::HistoryBar::HistoryBar()
{
}



Component::HistoryBar::~HistoryBar()
{
}



void Component::HistoryBar::PushButton(UINT id)
{
	RemoveButton(id);

	if (m_buttons.size() >= m_nMaxCount) {
		REMOVE_POINTER(m_buttons.front());
		m_buttons.erase(m_buttons.begin());
	}

	// insert front
	m_buttons.insert(m_buttons.begin(), CreateButton(id));
	AdjustLayout();
	ShowWindow(SW_SHOW);
}



void Component::HistoryBar::RemoveButton(UINT id)
{
	std::vector<CBCGPButton*>::iterator iter;
	for (iter = m_buttons.begin(); iter != m_buttons.end(); iter++) {
		auto button = *iter;
		if (button != nullptr && button->GetDlgCtrlID() == id) {
			REMOVE_POINTER(button);
			m_buttons.erase(iter);
			break;
		}
	}
}
