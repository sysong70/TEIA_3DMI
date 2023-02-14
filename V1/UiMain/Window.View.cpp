#include "stdafx.h"
#include "resource.h"
#include "Window.View.h"
#include "Window.Application.h"
#include "Window.Document.h"
#include "Connector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetView

namespace PresetView
{
	int ViewIndex = 0;
	const UINT_PTR ActivateDelayTimer = 1234567;

	enum EPanelId
	{
		TabId = WM_USER,
	};
}



using namespace Window;

IMPLEMENT_DYNCREATE(View, CView)

BEGIN_MESSAGE_MAP(View, CView)
	ON_WM_ACTIVATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEACTIVATE()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()

	ON_COMMAND_RANGE(COMMAND_START, COMMAND_END, OnCommand)
	ON_MESSAGE((UINT)EUserMessage::OnSignal, OnSignal)
	ON_WM_TIMER()
END_MESSAGE_MAP()



Window::View::View()
{
	m_nViewId = PRESET::ViewIndex++;

	GetMainFrame().ViewChanged(WM_CREATE, this);
}



Window::View::~View()
{
	m_tabs.DestroyWindow();
	m_delivery.view.OnDestruct();

	GetMainFrame().ViewChanged(WM_DESTROY, this);
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



void Window::View::ReceiveSignal(Json::Object* pData)
{
}



void Window::View::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	Activate(bActivate);

	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);
}



void Window::View::OnDraw(CDC* /*pDC*/)
{
	Document* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	//:TODO
}



void Window::View::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	m_delivery.view.OnInitialize((DWORD_PTR)m_hWnd, GetDocument()->GetFilePath());

	CreateHistoryBar();
	CreateToolBar();
	CreatePanelTabs();
	CreateTaskBar();
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
	CView::OnActivate(nState, pWndOther, bMinimized);

	Activate(nState == WA_ACTIVE);
}



void Window::View::OnCommand(UINT id)
{
	// global post process

	switch (id) {
	case FILE_3D_CMD_New:
	case FILE_3D_CMD_Open:
	case FILE_3D_CMD_Preference:
	case HOME_3D_CMD_Window_Cascade:
	case HOME_3D_CMD_Window_TileHorizontal:
	case HOME_3D_CMD_Window_TileVertical:
		GetMainFrame().OnCommand(id);
		return;

	case HOME_3D_CMD_Panels_Model:
	case HOME_3D_CMD_Panels_View:
	case HOME_3D_CMD_Panels_Layer:
	case HOME_3D_CMD_Panels_Scene:
		m_tabs.SetActiveTab(id - HOME_3D_PNL_Panels - 1);
		GetMainFrame().ShowPanelBar();
		return;

	default:
		if (m_bRenderer == false) {
			return;
		}
	}

	int pId = CBCGPRibbonPaletteButton::GetLastSelectedItem(id);
	id += (pId >= 0 ? pId : 0);
	m_historyBar.PushButton(id);

	Facility::CommandIndexer::Command& data = TheCommandIndexer.Get(id);

	if (data.Local) {
		//switch (id) {
		//default:
		//	break;
		//}
	}
	else {
		switch (data.Type) {
		case Facility::CommandIndexer::ListItem:
		case Facility::CommandIndexer::Check: //:TEMP
			m_delivery.view.OnCommand(id + pId);
			break;

		case Facility::CommandIndexer::Unknown:
			DEBUG_STOP;
			break;

		default:
			m_delivery.view.OnCommand(id);
			break;
		}
	}
}



void Window::View::OnContextMenu(CWnd*, CPoint point)
{
	if (CBCGPPopupMenu::GetSafeActivePopupMenu() != nullptr) {
		return;
	}

	TheAppication.ShowPopupMenu(IDR_CONTEXT_MENU, point, this);
}



void Window::View::OnPaint()
{
	//:WARNING - do not remove! call CPaintDC or CWindow::View::OnPaint() 
	CPaintDC dc(this);
	CRect rect = GetClientArea();

	if (m_bRenderer) {
		m_delivery.view.OnPaint(rect.left, rect.top, rect.right, rect.bottom);
	}
	else {
		dc.FillSolidRect(rect, (COLORREF)Component::EColor::Charcoal);
	}
}



void Window::View::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetCapture();
		m_delivery.view.OnLButtonDown(nFlags, point.x, point.y);
	}

	__super::OnLButtonDown(nFlags, point);
}



void Window::View::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		nFlags |= MK_LBUTTON;
		m_delivery.view.OnLButtonUp(nFlags, point.x, point.y);
		ReleaseCapture();
		SetCursor(TheAppication.LoadStandardCursor(IDC_ARROW));
	}

	__super::OnLButtonUp(nFlags, point);
}



void Window::View::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetCapture();
		m_delivery.view.OnLButtonDown(nFlags, point.x, point.y);
	}

	__super::OnMButtonDown(nFlags, point);
}



void Window::View::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		nFlags |= MK_MBUTTON;
		m_delivery.view.OnMButtonUp(nFlags, point.x, point.y);
		ReleaseCapture();
		SetCursor(TheAppication.LoadStandardCursor(IDC_ARROW));
	}

	__super::OnMButtonUp(nFlags, point);
}



int Window::View::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	int result = CView::OnMouseActivate(pDesktopWnd, nHitTest, message);

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
		m_delivery.view.OnMouseMove(nFlags, point.x, point.y);
	}

	__super::OnMouseMove(nFlags, point);
}



void Window::View::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetCapture();
		m_delivery.view.OnRButtonDown(nFlags, point.x, point.y);
	}

	__super::OnRButtonDown(nFlags, point);
}



void Window::View::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		nFlags |= MK_MBUTTON;
		m_delivery.view.OnRButtonUp(nFlags, point.x, point.y);
		ReleaseCapture();
		SetCursor(TheAppication.LoadStandardCursor(IDC_ARROW));
	}

	__super::OnRButtonUp(nFlags, point);
}



void Window::View::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		m_toolBar.AdjustLayout();
		m_historyBar.AdjustLayout();
		m_taskBar.AdjustLayout();
	}

	if (m_bRenderer) {
		m_delivery.view.OnResize(cx, cy);
	}
}




void Window::View::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == PRESET::ActivateDelayTimer) {
		KillTimer(nIDEvent);
		m_bActivate = true;
	}

	CView::OnTimer(nIDEvent);
}



void Window::View::Activate(bool value)
{
	if (m_bRenderer) {
		m_bActivate = value;

		if (value) {
			GetMainFrame().ViewChanged(WM_ACTIVATE, this);
			//DelayViewActivation();
			m_toolBar.ShowWindow(SW_SHOW);
			m_historyBar.ShowWindow(SW_SHOW);
		}
		else {
			m_toolBar.ShowWindow(SW_HIDE);
			m_historyBar.ShowWindow(SW_HIDE);
		}
	}
}



void Window::View::DelayViewActivation()
{
	SetTimer(PRESET::ActivateDelayTimer, 100, nullptr);
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
	m_historyBar.SetPivot(Component::EPivot::BottomCenter, false);
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

	m_tabs.SetTabHeight(Component::TabHeight());
	m_tabs.SetLocation(CBCGPTabWnd::LOCATION_TOP);
	m_tabs.SetIconLocation(CBCGPTabWnd::TAB_ICON_LEFT);
}



void Window::View::CreateTaskBar()
{
	m_taskBar.Initialize(this);
}



void Window::View::ShowTaskBar()
{
	m_taskBar.ShowWindow(SW_SHOW);
	m_taskBar.AdjustLayout();
}

#undef PRESET
