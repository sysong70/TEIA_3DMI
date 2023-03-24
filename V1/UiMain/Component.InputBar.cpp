#include "stdafx.h"
#include "Component.InputBar.h"
#include "Window.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetInputBar

namespace PresetInputBar
{
	const UINT Id = WM_USER;

	CSize ControlSize()
	{
		return globalUtils.ScaleByDPI(CSize(72, 20));
	}

	int Margin()
	{
		//return globalUtils.ScaleByDPI(2);
		return 0;
	}

	CPoint Offset()
	{
		return globalUtils.ScaleByDPI(CPoint(16, 16));
	}

	std::vector<CBCGPEdit*> Controls;
}



using namespace Component;

BEGIN_MESSAGE_MAP(InputBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()



Component::InputBar::InputBar()
{
	PRESET::Controls.push_back(&m_wndEdit1);
	PRESET::Controls.push_back(&m_wndEdit2);
	PRESET::Controls.push_back(&m_wndEdit3);
	PRESET::Controls.push_back(&m_wndEdit4);
}



Component::InputBar::~InputBar()
{
}



bool Component::InputBar::Initialize(Window::View* pView)
{
	if (m_pView != nullptr) {
		return true;
	}

	m_pView = pView;

	DWORD dwExStyle = WS_EX_COMPOSITED;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	CRect rect(0, 0,
		PRESET::ControlSize().cx * 4 + PRESET::Margin() * 3,
		PRESET::ControlSize().cy
	);
	BOOL success = __super::CreateEx(dwExStyle, NULL, L"", dwStyle, rect, m_pView, PRESET::Id);
	if (success == FALSE) {
		RETURN_FALSE;
	}

	UINT index = 0;
	CPoint offset;
	CSize size = PRESET::ControlSize();

	CBCGPEditColors colors;
	colors.m_clrBackground = (COLORREF)Control::EColor::DarkBack;

	for (auto input : PRESET::Controls) {
		input->m_bVisualManagerStyle = TRUE;
		input->m_bOnGlass = TRUE;
		input->SetColorTheme(colors, FALSE);

		DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP;
		if (input->Create(dwStyle, CRect(offset, size), this, PRESET::Id + index++) == FALSE) {
			RETURN_FALSE;
		}

		offset.x += size.cx + PRESET::Margin();
		input->SetFont(&globalUtils.GetFontRegular(pView));
	}

	ChangeActive(0);

	return true;
}



bool Component::InputBar::IsVisible()
{
	return m_bVisible;
}



void Component::InputBar::ChangeActive(int index)
{
	if (index >= PRESET::Controls.size()) {
		index = 0;
	}

	CBCGPEditColors colors;
	colors.m_clrBackground = (COLORREF)Control::EColor::DarkBack;
	PRESET::Controls[m_nActivated]->SetColorTheme(colors);

	m_nActivated = index;
	colors.m_clrBackground = (COLORREF)Control::EColor::MidiumBack;
	PRESET::Controls[m_nActivated]->SetColorTheme(colors);
	PRESET::Controls[m_nActivated]->SetSel(0, -1);
}



void Component::InputBar::Clear()
{
	for (auto input : PRESET::Controls) {
		input->SetWindowText(L"");
	}
}



void Component::InputBar::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (m_nActivated >= 0) {
		switch (nChar) {
		case VK_TAB:
			ChangeActive(m_nActivated + 1);
			break;

		default:
			PRESET::Controls[m_nActivated]->SendMessage(WM_CHAR, (WPARAM)nChar);
		}
	}
}



void Component::InputBar::OnMouseMove(UINT nFlags, CPoint point)
{
	if (GetSafeHwnd() != nullptr) {
		CRect rect;
		GetWindowRect(rect);

		point += PRESET::Offset();
		SetWindowPos(nullptr, point.x, point.y, 0, 0, SWP_NOSIZE);
	}
}



BOOL Component::InputBar::OnEraseBkgnd(CDC* pDC)
{
	return __super::OnEraseBkgnd(pDC);
}



void Component::InputBar::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);

	m_bVisible = bShow;
	if (m_bVisible == false) {
		Clear();
	}
}
