#include "stdafx.h"
#include "resource.h"
#include "Window.View.h"
#include "Window.Application.h"
#include "Window.Document.h"
#include "Command.Base.h"
#include "Connector.h"
#include "Facility.AppOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define PRESET PresetView

namespace PresetView
{
	int ViewIndex = 0;

	enum EPanelId
	{
		TabId = WM_USER,
	};
}

//**************************************************************************************************

using namespace Window;

IMPLEMENT_DYNCREATE(View, CView)

BEGIN_MESSAGE_MAP(View, CView)
END_MESSAGE_MAP()



Window::View::View()
{
	m_nViewId = PRESET::ViewIndex++;

	GetMainFrame().ViewChanged(WM_CREATE, this);
}



Window::View::~View()
{
	m_toolBar.DestroyWindow();
	m_historyBar.DestroyWindow();
	m_tabs.DestroyWindow();

	GetDelivery().view.OnDestruct();
	//:WARNING - change after OnDestruct()
	m_nViewId = -1;
	GetMainFrame().ViewChanged(WM_DESTROY, this);
}



Signal::Delivery& Window::View::GetDelivery()
{
	return Connector3d::GetInstance(m_nViewId);
}



void Window::View::CancelCommand()
{
	GetDelivery().view.OnCancel();

	if (m_pActiveCommand != nullptr) {
		m_pActiveCommand->Cancel();
		m_pActiveCommand = nullptr;
	}
}



Window::Document* Window::View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(Document)));
	return (Document*)m_pDocument;
}



int Window::View::GetId()
{
	return m_nViewId;
}



void Window::View::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	Activate(bActivate);

	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);
}



void Window::View::OnDraw(CDC* pDC)
{
}



void Window::View::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	CreateHistoryBar();
	CreateToolBar();
	CreatePanelTabs();

	GetDelivery().view.OnInitialize((DWORD_PTR)m_hWnd, GetDocument()->GetPathName());
}



BOOL Window::View::PreCreateWindow(CREATESTRUCT& cs)
{
	// Setup Window class to work with HPS rendering.
	// The REDRAW flags prevent flickering when resizing, and OWNDC allocates a single device context to for this window.
	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW);

	return __super::PreCreateWindow(cs);
}

#ifdef _DEBUG

void Window::View::AssertValid() const
{
	__super::AssertValid();
}



void Window::View::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG

LRESULT Window::View::OnSignal(WPARAM wp, LPARAM lp)
{
	ReceiveSignal((Json::Object*)wp);

	return 0;
}



void Window::View::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	__super::OnActivate(nState, pWndOther, bMinimized);

	Activate(nState == WA_ACTIVE);
}



void Window::View::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnChar(nChar, nRepCnt, nFlags);
}



void Window::View::OnContextMenu(CWnd*, CPoint point)
{
	if (CBCGPPopupMenu::GetSafeActivePopupMenu() != nullptr) {
		return;
	}

	TheApplication.ShowPopupMenu(IDR_CONTEXT_MENU, point, this);
}



BOOL Window::View::OnEraseBkgnd(CDC* pDC)
{
	//:WARING - do not remove background
	//return __super::OnEraseBkgnd(pDC);
	return TRUE;
}



void Window::View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	case VK_ESCAPE:
		CancelCommand();
		return;

	default:
		break;
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}



void Window::View::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	//:TODO

	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}



void Window::View::OnPaint()
{
	//:WARNING - do not remove! call CPaintDC or CWindow::View::OnPaint() 
	CPaintDC dc(this);
	CRect rect = GetClientArea();

	if (m_bRenderer) {
		GetDelivery().view.OnPaint(rect.left, rect.top, rect.right, rect.bottom);
	}
	else {
		dc.FillSolidRect(rect, (COLORREF)Control::EColor::DarkBack);
	}
}



void Window::View::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetCapture();

		//if (TheAppOptions.GetBoolean("Environment/Mouse/SwapPanAndRotate")) {
		//	nFlags |= (nFlags & ~MK_LBUTTON) | MK_RBUTTON;
		//	GetDelivery().view.OnRButtonDown(nFlags, point.x, point.y);
		//}
		//else {
			GetDelivery().view.OnLButtonDown(nFlags, point.x, point.y);
		//}
	}

	__super::OnLButtonDown(nFlags, point);
}



void Window::View::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		//if (TheAppOptions.GetBoolean("Environment/Mouse/SwapPanAndRotate")) {
		//	nFlags |= MK_RBUTTON;
		//	GetDelivery().view.OnRButtonUp(nFlags, point.x, point.y);
		//}
		//else {
			nFlags |= MK_LBUTTON;
			GetDelivery().view.OnLButtonUp(nFlags, point.x, point.y);
		//}

		ReleaseCapture();
		SetCursor(TheApplication.LoadStandardCursor(IDC_ARROW));
	}

	__super::OnLButtonUp(nFlags, point);
}



void Window::View::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetCapture();

		GetDelivery().view.OnLButtonDown(nFlags, point.x, point.y);
	}

	__super::OnMButtonDown(nFlags, point);
}



void Window::View::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		nFlags |= MK_MBUTTON;
		GetDelivery().view.OnMButtonUp(nFlags, point.x, point.y);

		ReleaseCapture();
		SetCursor(TheApplication.LoadStandardCursor(IDC_ARROW));
	}

	__super::OnMButtonUp(nFlags, point);
}



int Window::View::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	int result = __super::OnMouseActivate(pDesktopWnd, nHitTest, message);

	if (m_bActivate) {
		return result;
	}
	else {
		// disable mouse action
		return MA_ACTIVATEANDEAT;
	}
}



void Window::View::OnMouseMove(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		GetDelivery().view.OnMouseMove(nFlags, point.x, point.y);
	}

	__super::OnMouseMove(nFlags, point);
}



void Window::View::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetCapture();

		//if (TheAppOptions.GetBoolean("Environment/Mouse/SwapPanAndRotate")) {
		//	nFlags != (nFlags & ~MK_LBUTTON) | MK_RBUTTON;
		//	GetDelivery().view.OnLButtonDown(nFlags, point.x, point.y);
		//}
		//else {
		GetDelivery().view.OnRButtonDown(nFlags, point.x, point.y);
		//}
	}

	__super::OnRButtonDown(nFlags, point);
}



void Window::View::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		//if (TheAppOptions.GetBoolean("Environment/Mouse/SwapPanAndRotate")) {
		//	nFlags |= MK_LBUTTON;
		//	GetDelivery().view.OnLButtonUp(nFlags, point.x, point.y);
		//}
		//else {
			nFlags |= MK_RBUTTON;
			GetDelivery().view.OnRButtonUp(nFlags, point.x, point.y);
		//}

		ReleaseCapture();
		SetCursor(TheApplication.LoadStandardCursor(IDC_ARROW));
	}

	__super::OnRButtonUp(nFlags, point);
}



void Window::View::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		m_toolBar.AdjustLayout();
		m_historyBar.AdjustLayout();
		TheApplication.GetMainFrame().m_taskBar.AdjustLayout();
	}

	if (m_bRenderer) {
		GetDelivery().view.OnResize(cx, cy);
	}
}



void Window::View::Activate(bool value)
{
	if (m_bRenderer) {
		m_bActivate = value;

		if (value) {
			GetMainFrame().ViewChanged(WM_ACTIVATE, this);
			m_toolBar.ShowWindow(SW_SHOW);
			m_historyBar.ShowWindow(SW_SHOW);
		}
		else {
			m_toolBar.ShowWindow(SW_HIDE);
			m_historyBar.ShowWindow(SW_HIDE);
		}
	}
}



CRect Window::View::GetClientArea()
{
	CRect rect;
	GetClientRect(rect);

	return rect;
}



CSize Window::View::GetClientSize()
{
	return GetClientArea().Size();
}



Window::MainFrame& Window::View::GetMainFrame()
{
	return *(Window::MainFrame*)AfxGetMainWnd();
}



bool Window::View::IsValid()
{
	return m_bRenderer && m_bActivate;
}




void Window::View::CreateHistoryBar()
{
	m_historyBar.SetPivot(Control::EPivot::BottomCenter, false);
	m_historyBar.Initialize(this);
}



void Window::View::CreateToolBar()
{
	m_toolBar.Initialize(this);
}



void Window::View::CreatePanelTabs()
{
	if (m_tabs.Create(CBCGPTabWnd::STYLE_3D, {}, this, PRESET::TabId) == FALSE) {
		DEBUG_RETURN;
	}

	m_tabs.SetTabHeight(Control::TabHeight());
	m_tabs.SetLocation(CBCGPTabWnd::LOCATION_TOP);
	m_tabs.SetIconLocation(CBCGPTabWnd::TAB_ICON_LEFT);
}

#undef PRESET
