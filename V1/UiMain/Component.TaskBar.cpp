#include "stdafx.h"
#include "Component.TaskBar.h"
#include "Control.TaskPanel.h"
#include "Facility.AppResources.h"
#include "Window.MainFrame.h"
#include "Window.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetTaskBar

namespace PresetTaskBar
{
	enum EControlId
	{
		Id = WM_USER,
	};

	CRect BarMargin()
	{
		return globalUtils.ScaleByDPI(CRect(4, 38, 0, 38));
	}

	CSize CloseHandleSize()
	{
		return globalUtils.ScaleByDPI(CSize(16, 16));
	}
}



using namespace Component;

BEGIN_MESSAGE_MAP(TaskBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)

	ON_BN_CLICKED(IDCLOSE, OnClose)
END_MESSAGE_MAP()



Component::TaskBar::TaskBar()
{
}



Component::TaskBar::~TaskBar()
{
	DestroyWindow();
}



bool Component::TaskBar::Initialize(CWnd* pMainFrame)
{
	const DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, {}, pMainFrame, PRESET::Id) == FALSE) {
		RETURN_FALSE;
	}

	m_nHeaderHeight = BarHeaderHeight();
	m_nFooterHeight = BarFooterHeight();

	// Header

	m_wndTitle.Create(L"TITLE", WS_CHILD | WS_VISIBLE, {}, this);
	m_wndTitle.m_bVisualManagerStyle = TRUE;
	
	m_wndCloseHandle.Create(L"✕", WS_CHILD | WS_VISIBLE, globalUtils.ScaleByDPI(CRect(0, 0, 16, 16)), this, IDCLOSE);
	m_wndCloseHandle.m_bVisualManagerStyle = TRUE;

	// Footer

	Json::Object& data = TheAppResources.GetDialog("DefaultButtons").GetAt("Close");
	m_wndClose.Create(Facility::GetTitle(data), WS_CHILD | WS_VISIBLE, {}, this, Facility::GetId(data));
	m_wndClose.m_bVisualManagerStyle = TRUE;
	m_wndClose.SizeToContent();

	Control::AdjustSize(&m_wndClose, globalUtils.ScaleByDPI(CSize(64, 0)));

	return true;
}



CSize Component::TaskBar::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr || IsWindowVisible() == false ||
		GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr ||
		m_pPanel == nullptr) {
		return {};
	}

	int padding = BarPadding();
	CRect margin = PRESET::BarMargin();

	CRect winRect;
	GetParent()->GetClientRect(winRect);

	CSize winSize = winRect.Size();
	CSize barSize = m_pPanel->GetSize() + CSize(padding * 2, m_nHeaderHeight + m_nFooterHeight);
	CSize maxSize = { winSize.cx - margin.left - margin.right, winSize.cy - margin.top - margin.bottom };

	maxSize.cx = min(maxSize.cx, barSize.cx);
	maxSize.cy = min(maxSize.cy, barSize.cy);

	CRect headerRect, bodyRect, footerRect;
	GetArea(headerRect, bodyRect, footerRect);

	// Headeer

	Control::AdjustPosition(&m_wndCloseHandle, headerRect, Control::EPivot::TopRight);

	headerRect.right -= PRESET::CloseHandleSize().cx;
	Control::AdjustPosition(&m_wndTitle, headerRect, Control::EPivot::TopLeft);

	// Body

	m_pPanel->SetWindowPos(NULL, bodyRect.left, bodyRect.top, bodyRect.Width(), bodyRect.Height(), SWP_SHOWWINDOW);

	// Footer
	Control::AdjustPosition(&m_wndClose, footerRect, Control::EPivot::BottomRight);

	SetWindowPos(NULL, margin.left, margin.top, maxSize.cx, maxSize.cy, SWP_SHOWWINDOW);

	return maxSize;
}



Signal::Delivery& Component::TaskBar::GetDelivery()
{
	return GetView()->GetDelivery();
}



Window::View* Component::TaskBar::GetView()
{
	return (Window::View*)GetParent();
}



void Component::TaskBar::SetPanel(Control::TaskPanel* pPanel)
{
	m_wndTitle.SetWindowText(pPanel->GetTitle());
	m_wndTitle.SizeToContent();

	m_pPanel = pPanel;
}



void Component::TaskBar::Show(Window::View* pTargetView)
{
	if (pTargetView == nullptr) {
		if (m_pPanel != nullptr) {
			m_pPanel->DestroyWindow();
			REMOVE_POINTER(m_pPanel);
		}

		ShowWindow(SW_HIDE);
		return;
	}

	SetParent((CWnd*)pTargetView);
	ShowWindow(SW_SHOW);
	AdjustLayout();
}



void Component::TaskBar::GetArea(CRect& header, CRect& body, CRect& footer)
{
	int padding = BarPadding();
	CRect margin = PRESET::BarMargin();

	CRect viewRect;
	GetParent()->GetClientRect(viewRect);

	CSize viewSize = viewRect.Size();
	CSize barSize = m_pPanel->GetSize() + CSize(padding * 2, m_nHeaderHeight + m_nFooterHeight);
	CSize maxSize = { viewSize.cx - margin.left - margin.right, viewSize.cy - margin.top - margin.bottom };

	maxSize.cx = min(maxSize.cx, barSize.cx);
	maxSize.cy = min(maxSize.cy, barSize.cy);

	header = { padding, padding, barSize.cx - padding, m_nHeaderHeight * padding * 2 };
	footer = { padding, maxSize.cy - m_nFooterHeight - padding, barSize.cx - padding, maxSize.cy - padding };
	body = { padding, m_nHeaderHeight, header.right, footer.top };
}



void Component::TaskBar::OnClose()
{
	DEBUG_VALID(GetParent());
	GetParent()->SendMessage(WM_KEYDOWN, VK_ESCAPE, 0);
}



LRESULT Component::TaskBar::OnDPIChangedAfterParent(WPARAM, LPARAM)
{
	LRESULT result = Default();

	//:TODO

	return result;
}



BOOL Component::TaskBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)Control::EColor::DialogBack);

	CRect rect;
	GetClientRect(rect);

	pDC->FillRect(rect, (CBrush*)&CBrush((COLORREF)Control::EColor::Gray));
	rect.DeflateRect(CRect(1, 1, 1, 1));
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}

#undef PRESET
