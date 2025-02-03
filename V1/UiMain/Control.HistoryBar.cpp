#include "stdafx.h"
#include "resource.h"
#include "Control.HistoryBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

Control::HistoryBar::HistoryBar()
{
}



Control::HistoryBar::~HistoryBar()
{
}



void Control::HistoryBar::PushButton(UINT id)
{
	RemoveButton(id);

	if (m_buttons.size() >= m_nMaxCount) {
		REMOVE_POINTER(m_buttons.back());
		m_buttons.pop_back();
	}

	m_buttons.push_back(CreateButton(id, false, false));
	AdjustLayout();
	ShowWindow(SW_SHOW);
}



void Control::HistoryBar::RemoveButton(UINT id)
{
	std::vector<CBCGPButton*>::iterator it;
	for (it = m_buttons.begin(); it != m_buttons.end(); it++) {
		auto button = *it;
		if (button != nullptr && button->GetDlgCtrlID() == id) {
			REMOVE_POINTER(button);
			m_buttons.erase(it);
			break;
		}
	}
}
