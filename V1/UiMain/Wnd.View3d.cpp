#include "stdafx.h"

#include "Ast.h"
#include "Ast.AppOptions.h"
#include "Cmd.Base.h"
#include "Cmd.Indexer.h"
#include "Cnt.h"
#include "Wnd.Application.h"
#include "Wnd.Document.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View3d.h"

#include <Signal3d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define theDelivery		Cnt3d::GetInstance(ViewId)
#define theMainFrame	TheApp.GetMainFrame()



namespace
{
	int ViewIndex = 0;
}

//**************************************************************************************************

IMPLEMENT_DYNCREATE(WndView3d, WndView)

BEGIN_MESSAGE_MAP(WndView3d, WndView)
	ON_WM_ACTIVATE()
	ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
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
	ON_WM_TIMER()

	ON_COMMAND_RANGE(COMMAND_START, COMMAND_END, OnCommand)
	ON_UPDATE_COMMAND_UI_RANGE(COMMAND_START, COMMAND_END, OnUpdateCommand)
	ON_MESSAGE((UINT)Wnd::EMessage::OnSignal3d, OnSignal)
END_MESSAGE_MAP()



WndView3d::WndView3d()
	: WndView()
{
	ViewType = Wnd::EViewType::View3d;

	theDelivery.view.OnConstruct();
}



WndView3d::~WndView3d()
{
	theDelivery.view.OnDestruct();
}



void WndView3d::Activate(bool value)
{
#define ShowOrHide(x) if (x.GetSafeHwnd() != nullptr) x.ShowWindow(cmd);

	Activated = value;
	int cmd = value ? SW_SHOW : SW_HIDE;

	ShowOrHide(ToolBarCtl);
	ShowOrHide(HistoryBarCtl);

	theMainFrame.ViewChanged(WM_ACTIVATE, this);

#undef ShowOrHide
}



void WndView3d::CreateToolBar()
{
	__super::CreateToolBar();

	ToolBarCtl.AddButtons({
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
	});
}



Signal::Delivery* WndView3d::GetDelivery3d()
{
	return &Cnt3d::GetInstance(ViewId);
}



bool WndView3d::IsValid()
{
	// KEN - 20250124, OnPaint error
	//return ValidRenderer && Activated;
	return ValidRenderer;
}



void WndView3d::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;
	Signal::Target target = (Signal::Target)data.GetInteger(SKW_TARGET);

	switch (target) {
	case Signal::Target::ModelPanel: {
		PanelManager.ModelPanel.ReceiveSignal(pData);
		return;
	} break;

	case Signal::Target::View: {
		Signal::View::Action action = (Signal::View::Action)data.GetInteger(SKW_ACTION);

		switch (action) {
		case Signal::View::Action::SetValidation: {
			ValidRenderer = data.GetBoolean(SKW_VALID);

			if (ValidRenderer) {
				CRect rect = GetClientArea();
				theDelivery.view.OnPaint(rect.left, rect.top, rect.right, rect.bottom);

				if (theMainFrame.HasNextFile()) {
					theMainFrame.PostMessage((UINT)Wnd::EMessage::OnNextFileOpen);
				}
				else {
					SendMessage(WM_ACTIVATE, (WPARAM)WA_ACTIVE);
				}
			}
			else {
				GetDocument()->OnCloseDocument();
			}
		} break;

		default:
			DEBUG_STOP;
			break;
		}
	} break;
						
	case Signal::Target::TaskBar: {
		ASSERT(data.GetInteger(SKW_VIEWID, -1) == ViewId);

		if (ActiveCommand != nullptr) {
			ActiveCommand->ReceiveSignal(pData);
			return;
		}
		else {
			DEBUG_STOP;
		}
	} break;

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}



void WndView3d::CancelCommand()
{
	theDelivery.view.OnCancel();

	if (ActiveCommand != nullptr) {
		ActiveCommand->Cancel();
		ActiveCommand = nullptr;
	}
}



void WndView3d::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	CreateHistoryBar();
	CreateToolBar();
	PanelManager.Initialize(this, WM_USER);

	theDelivery.view.OnInitialize((DWORD_PTR)m_hWnd, GetDocument()->GetPathName());
}



BOOL WndView3d::PreCreateWindow(CREATESTRUCT& cs)
{
	// Setup Window class to work with HPS rendering.
	// The REDRAW flags prevent flickering when resizing,
	// and OWNDC allocates a single device context to for this window.
	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW);

	return __super::PreCreateWindow(cs);
}



void WndView3d::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnChar(nChar, nRepCnt, nFlags);
}



void WndView3d::OnCommand(UINT id)
{
	// global post process

	if (theMainFrame.HasCommandHandeler(id)) {
		theMainFrame.OnCommand(id);
		return;
	}

	switch (id) {
	case HOME_3D_CMD_Panels_Model:
	case HOME_3D_CMD_Panels_View:
	case HOME_3D_CMD_Panels_Layer:
	case HOME_3D_CMD_Panels_Scene:
		PanelManager.ActivatePanel(id - HOME_3D_PNL_Panels - 1);
		theMainFrame.ShowPanelBar();
		return;

	default:
		if (ValidRenderer == false) {
			return;
		}
	}

	CmdIndexer::CommandInfo cmd = TheCmdIndexer.Get(id);
	//id = (cmd.ChildId >= 0 ? cmd.ChildId : id);
	if (cmd.ChildId >= 0) {
		id = cmd.ChildId;
		// WARNING - replace info
		cmd = TheCmdIndexer.Get(id);
	}

	HistoryBarCtl.PushButton(id);
	CancelCommand();

	if (cmd.Function != nullptr) {
		CmdBase* pCommand = cmd.Function;
		if (pCommand->IsRunOnlyOnce() == false) {
			ActiveCommand = pCommand;
		}

		pCommand->Run(this);
	}
	else {
		switch (cmd.Type) {
		case CmdIndexer::Popup:
			break;

		case CmdIndexer::Unknown:
			DEBUG_STOP;
			break;

		case CmdIndexer::ListItem:
		case CmdIndexer::Check: // TEMP
		default:
			theDelivery.view.OnCommand(id);
			break;
		}
	}
}



void WndView3d::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsValid()) {
		theDelivery.view.OnKeyDown(nChar, nRepCnt, nFlags);
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}



void WndView3d::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}



void WndView3d::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetFocus();
		SetCapture();

		theDelivery.view.OnLButtonDown(nFlags, point.x, point.y);
	}

	__super::OnLButtonDown(nFlags, point);
}



void WndView3d::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		ReleaseCapture();
		SetCursor(TheApp.LoadStandardCursor(IDC_ARROW));

		//:WARNING - Do not remove
		nFlags |= MK_LBUTTON;
		theDelivery.view.OnLButtonUp(nFlags, point.x, point.y);
	}

	__super::OnLButtonUp(nFlags, point);
}



void WndView3d::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetFocus();
		SetCapture();

		theDelivery.view.OnMButtonDown(nFlags, point.x, point.y);
	}

	__super::OnMButtonDown(nFlags, point);
}



void WndView3d::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		ReleaseCapture();
		SetCursor(TheApp.LoadStandardCursor(IDC_ARROW));

		//:WARNING - Do not remove
		nFlags |= MK_MBUTTON;
		theDelivery.view.OnMButtonUp(nFlags, point.x, point.y);
	}

	__super::OnMButtonUp(nFlags, point);
}



int WndView3d::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	int result = __super::OnMouseActivate(pDesktopWnd, nHitTest, message);

	if (Activated) {
		return result;
	}
	else {
		// disable mouse action
		return MA_ACTIVATEANDEAT;
	}
}



void WndView3d::OnMouseMove(UINT nFlags, CPoint point)
{
	if (IsValid() && MousePoint != point) {
		theDelivery.view.OnMouseMove(nFlags, point.x, point.y);
	}

	MousePoint = point;

	__super::OnMouseMove(nFlags, point);
}



BOOL WndView3d::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	if (ValidRenderer) {
		CRect rect;
		GetWindowRect(rect);

		zDelta *= TheAppOptions.GetBoolean("Environment/Mouse/ReverseWheelDirection") ? -1 : 1;
		theDelivery.view.OnMouseWheel(nFlags, zDelta, point.x, point.y, rect.left, rect.top, rect.right, rect.bottom);
	}

	return WndView::OnMouseWheel(nFlags, zDelta, point);
}



void WndView3d::OnPaint()
{
	CPaintDC dc(this);
	CRect rect = GetClientArea();

	if (IsValid()) {
		theDelivery.view.OnPaint(rect.left, rect.top, rect.right, rect.bottom);
	}
	else {
		dc.FillSolidRect(rect, (COLORREF)Ctl::EColor::DarkBack);
	}
}



void WndView3d::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetFocus();
		SetCapture();

		theDelivery.view.OnRButtonDown(nFlags, point.x, point.y);
	}

	__super::OnRButtonDown(nFlags, point);
}



void WndView3d::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		ReleaseCapture();
		SetCursor(TheApp.LoadStandardCursor(IDC_ARROW));

		//:WARNING - Do not remove
		nFlags |= MK_RBUTTON;
		theDelivery.view.OnRButtonUp(nFlags, point.x, point.y);
	}

	__super::OnRButtonUp(nFlags, point);
}



void WndView3d::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		ToolBarCtl.AdjustLayout();
		HistoryBarCtl.AdjustLayout();
	}

	if (IsValid()) {
		theDelivery.view.OnResize(cx, cy);
	}
}



void WndView3d::OnUpdateCommand(CCmdUI* pCmdUI)
{
	/*
	for (auto id : m_checkedCommands) {
		if (id == pCmdUI->m_nID) {
			pCmdUI->SetCheck(TRUE);
			return;
		}
	}

	pCmdUI->SetCheck(FALSE);
	*/
}

#undef theDelivery
#undef theMainFrame
