#include "stdafx.h"

#include "Ctl.TaskBar.h"
#include "Ctl.TaskPanel.h"
#include "Ast.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	CRect BarMargin()
	{
		return globalUtils.ScaleByDPI(CRect(4, 38, 0, 38));
	}

	CSize CloseHandleSize()
	{
		return globalUtils.ScaleByDPI(CSize(16, 16));
	}
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlTaskBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)

	ON_BN_CLICKED(IDCLOSE, OnClose)
END_MESSAGE_MAP()



bool CtlTaskBar::Initialize(CWnd* pMainFrame)
{
	const DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, {}, pMainFrame, WM_USER) == FALSE) {
		RETURN_FALSE;
	}

	HeaderHeight = Ctl::BarHeaderHeight();
	FooterHeight = Ctl::BarFooterHeight();

	// Header

	TitleCtl.Create(L"TITLE", WS_CHILD | WS_VISIBLE, {}, this);
	TitleCtl.m_bVisualManagerStyle = TRUE;
	
	CloseCtl.Create(L"✕", WS_CHILD | WS_VISIBLE, globalUtils.ScaleByDPI(CRect(0, 0, 16, 16)), this, IDCLOSE);
	CloseCtl.m_bVisualManagerStyle = TRUE;

	// Footer

	Json::Object& data = TheAppResources.GetDialog("DefaultButtons").GetAt("Close");
	CloseButtonCtl.Create(Ast::GetTitle(data), WS_CHILD | WS_VISIBLE, {}, this, Ast::GetId(data));
	CloseButtonCtl.m_bVisualManagerStyle = TRUE;
	CloseButtonCtl.SizeToContent();

	Ctl::AdjustSize(&CloseButtonCtl, globalUtils.ScaleByDPI(CSize(64, 0)));

	return true;
}



CSize CtlTaskBar::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr || IsWindowVisible() == false ||
		GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr ||
		TaskPanelCtl == nullptr) {
		return {};
	}

	int padding = Ctl::BarPadding();
	CRect margin = BarMargin();

	CRect winRect;
	GetParent()->GetClientRect(winRect);

	CSize winSize = winRect.Size();
	CSize barSize = TaskPanelCtl->GetSize() + CSize(padding * 2, HeaderHeight + FooterHeight);
	CSize maxSize = { winSize.cx - margin.left - margin.right, winSize.cy - margin.top - margin.bottom };

	maxSize.cx = min(maxSize.cx, barSize.cx);
	maxSize.cy = min(maxSize.cy, barSize.cy);

	CRect headerRect, bodyRect, footerRect;
	GetArea(headerRect, bodyRect, footerRect);

	// Headeer

	Ctl::AdjustPosition(&CloseCtl, headerRect, Ctl::EPivot::TopRight);

	headerRect.right -= CloseHandleSize().cx;
	Ctl::AdjustPosition(&TitleCtl, headerRect, Ctl::EPivot::TopLeft);

	// Body

	TaskPanelCtl->SetWindowPos(NULL, bodyRect.left, bodyRect.top, bodyRect.Width(), bodyRect.Height(), SWP_SHOWWINDOW);

	// Footer
	Ctl::AdjustPosition(&CloseButtonCtl, footerRect, Ctl::EPivot::BottomRight);

	SetWindowPos(NULL, margin.left, margin.top, maxSize.cx, maxSize.cy, SWP_SHOWWINDOW);

	return maxSize;
}



void CtlTaskBar::SetPanel(CtlTaskPanel* pPanel)
{
	TitleCtl.SetWindowText(pPanel->GetTitle());
	TitleCtl.SizeToContent();

	TaskPanelCtl = pPanel;
}



void CtlTaskBar::Show(WndView* pTargetView)
{
	if (pTargetView == nullptr) {
		if (TaskPanelCtl != nullptr) {
			TaskPanelCtl->DestroyWindow();
			REMOVE_POINTER(TaskPanelCtl);
		}

		ShowWindow(SW_HIDE);
		return;
	}

	SetParent((CWnd*)pTargetView);
	ShowWindow(SW_SHOW);
	AdjustLayout();
}



void CtlTaskBar::GetArea(CRect& header, CRect& body, CRect& footer)
{
	int padding = Ctl::BarPadding();
	CRect margin = BarMargin();

	CRect viewRect;
	GetParent()->GetClientRect(viewRect);

	CSize viewSize = viewRect.Size();
	CSize barSize = TaskPanelCtl->GetSize() + CSize(padding * 2, HeaderHeight + FooterHeight);
	CSize maxSize = { viewSize.cx - margin.left - margin.right, viewSize.cy - margin.top - margin.bottom };

	maxSize.cx = min(maxSize.cx, barSize.cx);
	maxSize.cy = min(maxSize.cy, barSize.cy);

	header = { padding, padding, barSize.cx - padding, HeaderHeight * padding * 2 };
	footer = { padding, maxSize.cy - FooterHeight - padding, barSize.cx - padding, maxSize.cy - padding };
	body = { padding, HeaderHeight, header.right, footer.top };
}



void CtlTaskBar::OnClose()
{
	DEBUG_VALID(GetParent());
	GetParent()->SendMessage(WM_KEYDOWN, VK_ESCAPE, 0);
}



BOOL CtlTaskBar::PreTranslateMessage(MSG* pMsg)
{
	BOOL process = __super::PreTranslateMessage(pMsg);
	if (process == FALSE && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) {
		PostMessage(WM_COMMAND, IDCLOSE);
	}

	return process;
}



LRESULT CtlTaskBar::OnDPIChangedAfterParent(WPARAM, LPARAM)
{
	LRESULT result = Default();

	// TODO

	return result;
}



BOOL CtlTaskBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush border((COLORREF)Ctl::EColor::Gray);
	const CBrush backgound((COLORREF)Ctl::EColor::DialogBack);

	CRect rect;
	GetClientRect(rect);

	pDC->FillRect(rect, (CBrush*)&border);
	rect.DeflateRect(CRect(1, 1, 1, 1));
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}
