#include "stdafx.h"
#include "resource.h"
#include "Window.View.h"
#include "Window.Application.h"
#include "Window.Document.h"
#include "Connector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



#define PRESET PresetView

namespace PresetView
{
	int ViewIndex = 0;
	const UINT_PTR ActivateDelayTimer = 1234567;
	const UINT TabId = WM_USER + 1;

	enum class PanelId
	{
		ModelTree = TabId + 1,
		View,
		Layer,
		Scene,
	};

	CSize TabImageSize()
	{
		return globalUtils.ScaleByDPI(CSize(24, 24));
	}
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
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()

	ON_COMMAND_RANGE(COMMAND_START, COMMAND_END, OnCommand)
	ON_MESSAGE((UINT)UserMessage::OnSignal, OnSignal)
	ON_WM_TIMER()
END_MESSAGE_MAP()



Window::View::View()
{
	m_nViewId = PRESET::ViewIndex++;

	m_delivery.ViewId = m_nViewId;
	m_delivery.SetSender(Connector3d::GetSender());
	m_delivery.view.OnConstruct();

	GetMainFrame().ViewChanged(WM_CREATE, this);
}



Window::View::~View()
{
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
	Json::Object& data = *pData;
	Signal::View::Action action = (Signal::View::Action)data.GetInteger(SKW_ACTION);

	switch (action) {
	case Signal::View::Action::SetValidation:
		m_bValid = data.GetBoolean(SKW_VALID);
		if (m_bValid) {
			CRect rect = GetClientArea();
			m_delivery.view.OnResize(rect.Width(), rect.Height());

			if (GetMainFrame().HasNextFile()) {
				GetMainFrame().PostMessage((UINT)UserMessage::OnNextFileOpen);
			}
			else {
				SendMessage(WM_ACTIVATE, (WPARAM)WA_ACTIVE);
			}
		}
		else {
			GetDocument()->OnCloseDocument();
		}
		break;

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}



void Window::View::SetFilePath(CString s)
{
	m_sFilePath = s;
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

	m_delivery.view.OnInitialize((DWORD_PTR)m_hWnd, m_sFilePath);

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
	case HOME_3D_CMD_Window_Cascade:
	case HOME_3D_CMD_Window_TileHorizontal:
	case HOME_3D_CMD_Window_TileVertical:
		GetMainFrame().OnCommand(id);
		return;

	case HOME_3D_CMD_Panels_ModelTree:
	case HOME_3D_CMD_Panels_View:
	case HOME_3D_CMD_Panels_Layer:
	case HOME_3D_CMD_Panels_Scene:
		m_tabs.SetActiveTab(id - HOME_3D_PNL_Panels - 1);
		GetMainFrame().ShowPanelBar();
		return;

	default:
		if (m_bValid == false) {
			return;
		}
	}

	int pId = CBCGPRibbonPaletteButton::GetLastSelectedItem(id);
	id += (pId >= 0 ? pId : 0);
	m_historyBar.PushButton(id);

	return;

	Facility::CommandIndexer::Command& data = TheCommandIndexer.Get(id);

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



void Window::View::OnContextMenu(CWnd*, CPoint point)
{
	if (CBCGPPopupMenu::GetSafeActivePopupMenu() != NULL) {
		return;
	}

	TheAppication.ShowPopupMenu(IDR_CONTEXT_MENU, point, this);
}



void Window::View::OnPaint()
{
	//:WARNING - do not remove! call CPaintDC or CWindow::View::OnPaint() 
	CPaintDC dc(this);
	CRect rect = GetClientArea();

	if (m_bValid) {
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



BOOL Window::View::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	if (IsValid()) {
		CRect rect;
		GetWindowRect(rect);
		//ScreenToClient(&point);
		m_delivery.view.OnMouseWheel(nFlags, zDelta,
			point.x, point.y, rect.left, rect.top, rect.right, rect.bottom);
	}

	return __super::OnMouseWheel(nFlags, zDelta, point);
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

	if (m_bValid) {
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
	if (m_bValid) {
		if (value) {
			GetMainFrame().ViewChanged(WM_ACTIVATE, this);
			DelayViewActivation();
			m_toolBar.ShowWindow(SW_SHOW);
			m_historyBar.ShowWindow(SW_SHOW);
		}
		else {
			m_bActivate = false;
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



Window::MainFrame& Window::View::GetMainFrame()
{
	return *(Window::MainFrame*)AfxGetMainWnd();
}



bool Window::View::IsValid()
{
	return m_bValid && m_bActivate;
}



void Window::View::CreateHistoryBar()
{
	m_historyBar.SetPivot(Component::EPivot::BottomCenter, false);
	m_historyBar.Initialize(this);
}



void Window::View::CreateToolBar()
{
	m_toolBar.Initialize(this);

	m_toolBar.AddButtons({
		HOME_3D_CMD_Pan,
		HOME_3D_LST_Zoom,
		HOME_3D_LST_Rotate,
		0,
		HOME_3D_LST_ViewStyle,
		HOME_3D_LST_ViewDirection,
		HOME_3D_LST_Visualize,
		HOME_3D_LST_VisualEffects,
		0,
		HOME_3D_LST_Select,
		HOME_3D_POP_SelectionFiter,
		HOME_3D_POP_ObjectSanp
	});
}



void Window::View::CreatePanelTabs()
{
	//:WARNING - setting before create()
	m_tabs.SetImageSize(PRESET::TabImageSize());

	CRect dummy;
	if (m_tabs.Create(CBCGPTabWnd::STYLE_3D, dummy, this, PRESET::TabId) == FALSE) {
		DEBUG_RETURN;
	}

	m_modelTreePanel.Initialize(&m_tabs, (UINT)PRESET::PanelId::ModelTree);
	m_viewPanel.Initialize(&m_tabs, (UINT)PRESET::PanelId::View);
	m_layerPanel.Initialize(&m_tabs, (UINT)PRESET::PanelId::Layer);
	m_scenePanel.Initialize(&m_tabs, (UINT)PRESET::PanelId::Scene);

	m_tabs.SetLocation(CBCGPTabWnd::LOCATION_TOP);
	m_tabs.SetIconLocation(CBCGPTabWnd::TAB_ICON_LEFT);
	m_tabs.AddImages({
		HOME_3D_CMD_Panels_ModelTree,
		HOME_3D_CMD_Panels_View,
		HOME_3D_CMD_Panels_Layer,
		HOME_3D_CMD_Panels_Scene,
	});

	int image = 0;
	m_tabs.AddTab(&m_modelTreePanel, Facility::GetTitle(HOME_3D_CMD_Panels_ModelTree), image++);
	m_tabs.AddTab(&m_viewPanel, Facility::GetTitle(HOME_3D_CMD_Panels_View), image++);
	m_tabs.AddTab(&m_layerPanel, Facility::GetTitle(HOME_3D_CMD_Panels_Layer), image++);
	m_tabs.AddTab(&m_scenePanel, Facility::GetTitle(HOME_3D_CMD_Panels_Scene), image++);

	m_tabs.SetActiveTab(0);
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
