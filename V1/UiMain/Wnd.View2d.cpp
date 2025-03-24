#include "stdafx.h"

#include "Ast.h"
#include "Ast.AppOptions.h"
#include "Cmd.Base.h"
#include "Cmd.Indexer.h"
#include "Cnt.h"
#include "Wnd.Application.h"
#include "Wnd.Document.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View2d.h"

#include <Signal2d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define theDelivery		Cnt2d::GetInstance(ViewId)
#define theMainFrame	TheApp.GetMainFrame()

//**************************************************************************************************

IMPLEMENT_DYNCREATE(WndView2d, CView)

BEGIN_MESSAGE_MAP(WndView2d, CView)
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
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_TIMER()

	// CHECK - set range
	ON_COMMAND_RANGE(CONTEXT_2D_START, CONTEXT_2D_END, OnContextCommand)
	ON_COMMAND_RANGE(COMMAND_START, COMMAND_END, OnCommand)
	ON_UPDATE_COMMAND_UI_RANGE(COMMAND_START, COMMAND_END, OnUpdateCommand)
	ON_MESSAGE((UINT)Wnd::EMessage::OnSignal2d, OnSignal)
END_MESSAGE_MAP()



WndView2d::WndView2d()
	: WndView()
{
	ViewType = Wnd::EViewType::View2d;
	theDelivery.View.OnConstruct();
}



WndView2d::~WndView2d()
{
	theDelivery.View.OnDestruct();
}



void WndView2d::Activate(bool value)
{
#define ShowOrHide(x) if (x.GetSafeHwnd() != nullptr) x.ShowWindow(cmd);

	Activated = value;
	int cmd = value ? SW_SHOW : SW_HIDE;

	ShowOrHide(ToolBarCtl);
	ShowOrHide(HistoryBarCtl);

	theMainFrame.ViewChanged(WM_ACTIVATE, this);

#undef ShowOrHide
}



void WndView2d::CancelCommand()
{
	theDelivery.View.OnCancel();

	if (ActiveCommand != nullptr) {
		ActiveCommand->Cancel();
		ActiveCommand = nullptr;
	}
}



void WndView2d::CreateToolBar()
{
	__super::CreateToolBar();

	ToolBarCtl.AddButtons({
		HOME_2D_CMD_Pan,
		HOME_2D_CMD_Zoom_Fit,
	});
}



SgnDelivery2d* WndView2d::GetDelivery2d()
{
	return &Cnt2d::GetInstance(ViewId);
}



bool WndView2d::IsValid()
{
	// KEN - 20250124, OnPaint error
	//return ValidRenderer && Activated;
	return ValidRenderer;
}



void WndView2d::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;
	Sgn::ETarget target = (Sgn::ETarget)data.GetInteger(SKW_TARGET);

	switch (target) {
	case Sgn::ETarget::View: {
		SgnView::Action action = (SgnView::Action)data.GetInteger(SKW_ACTION);

		switch (action) {
		case SgnView::Action::SetValidation: {
			ValidRenderer = data.GetBoolean(SKW_VALID);

			if (ValidRenderer) {
				CRect rect = GetClientArea();
				theDelivery.View.OnPaint(rect.left, rect.top, rect.right, rect.bottom);

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

		case SgnView::Action::PaintOverlap:
			//:CHECK
			Painting = false;
			break;

		default:
			DEBUG_STOP;
			break;
		}
	} break;

	case Sgn::ETarget::TaskBar: {
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



bool WndView2d::SetContextMenu(Json::Object* pData)
{
	//:TODO - move to CtlCommandBar or DynamicInput
	DEBUG_STOP;

	DEBUG_VALID(pData);
	Json::Object& data = *pData;
	CMenu& menu = theMainFrame.GetContextMenu(true);

	bool show = data.GetBoolean(SKW_SHOW);

	Json::Array& items = data.GetArray(SKW_ITEMS);
	for (auto item : items.GetBuffer()) {
		int id = item->AsInteger();

		if (id == -1) {
			menu.AppendMenu(MF_SEPARATOR);
		}
		else if (id == CONTEXT_2D_POP_ObjectSnap_Overrides) {
			CMenu osnapMenu;
			osnapMenu.CreatePopupMenu();

			for (int subId = CONTEXT_2D_CMD_ObjectSnap_Point; subId <= CONTEXT_2D_CMD_ObjectSnap_Near; subId++) {
				osnapMenu.AppendMenu(MF_STRING, subId, Ast::GetTitle(subId));
			}

			// WARNING - osnapMenu.Detach()
			menu.AppendMenu(MF_STRING | MF_POPUP, (UINT_PTR)osnapMenu.Detach(), Ast::GetTitle(id));
		}
		else {
			menu.AppendMenu(MF_STRING, id, Ast::GetTitle(id));
		}
	}

	return true;
}



void WndView2d::ShowContextMenu(Json::Object* pData)
{
	//:TODO - move to CtlCommandBar or DynamicInput
	DEBUG_STOP;

	SetContextMenu(pData);

	CPoint point;
	::GetCursorPos(&point);

	CMenu& menu = theMainFrame.GetContextMenu(false);
	UINT id = TheApp.GetContextMenuManager()->TrackPopupMenu(menu, point.x, point.y, this);
	if (id > 0) {
		OnContextCommand(id);
	}
}



void WndView2d::OnInitialUpdate()
{
	__super::OnInitialUpdate();

	CreateHistoryBar();
	CreateToolBar();
	PanelManager.Initialize(this, WM_USER);

	theDelivery.View.OnInitialize((DWORD_PTR)m_hWnd, GetDocument()->GetPathName());
}



BOOL WndView2d::PreCreateWindow(CREATESTRUCT& cs)
{
	return __super::PreCreateWindow(cs);
}



void WndView2d::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	theMainFrame.UioManager.OnChar(nChar, nRepCnt, nFlags);
}



void WndView2d::OnCommand(UINT id)
{
	// global post process

	if (theMainFrame.HasCommandHandeler(id)) {
		theMainFrame.OnCommand(id);
		return;
	}

	switch (id) {
	case HOME_2D_CMD_Panels_Model:
	case HOME_2D_CMD_Panels_View:
	case HOME_2D_CMD_Panels_Layer:
		DEBUG_STOP;
		return;

	case CUSTOM_3D_CMD_KEN_Test3:
		theDelivery.UserIO.OnInput(L"LINE");
		return;

	default:
		if (ValidRenderer == false) {
			return;
		}
	}

	CmdIndexer::CommandInfo* pInfo = &TheCmdIndexer.Get(id);
	DEBUG_VALID(pInfo);

	if (pInfo != nullptr && pInfo->ChildId >= 0) {
		id = pInfo->ChildId;
		// WARNING - replace info (do not reference valuable)
		pInfo = &TheCmdIndexer.Get(id);
		DEBUG_VALID(pInfo);
	}

	// TEST - move to CtlCommandBar
	//HistoryBarCtl.PushButton(id);
	CancelCommand();

	if (pInfo->Function != nullptr) {
		CmdBase* pCommand = pInfo->Function;
		if (pCommand->IsRunOnlyOnce() == false) {
			ActiveCommand = pCommand;
		}

		pCommand->Run(this);
	}
	else {
		switch (pInfo->Type) {
			case CmdIndexer::Popup:
				break;

			case CmdIndexer::Unknown:
				DEBUG_STOP;
				break;

			case CmdIndexer::ListItem:
			case CmdIndexer::Check: // TEMP
			default:
				theDelivery.View.OnCommand(id);
				break;
		}
	}
}



void WndView2d::OnContextCommand(UINT id)
{
	theDelivery.View.OnContextCommand(id);
}



void WndView2d::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsValid()) {
		theDelivery.View.OnKeyDown(nChar, nRepCnt, nFlags);
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}



void WndView2d::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnKeyUp(nChar, nRepCnt, nFlags);
}



void WndView2d::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetFocus();
		SetCapture();

		theDelivery.View.OnLButtonDown(nFlags, point.x, point.y);
	}

	__super::OnLButtonDown(nFlags, point);
}



void WndView2d::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		ReleaseCapture();
		SetCursor(TheApp.LoadStandardCursor(IDC_ARROW));

		//:WARNING - Do not remove
		nFlags |= MK_LBUTTON;
		theDelivery.View.OnLButtonUp(nFlags, point.x, point.y);
	}

	__super::OnLButtonUp(nFlags, point);
}



void WndView2d::OnMButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetFocus();
		SetCapture();

		theDelivery.View.OnMButtonDown(nFlags, point.x, point.y);
	}

	__super::OnMButtonDown(nFlags, point);
}



void WndView2d::OnMButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		ReleaseCapture();
		SetCursor(TheApp.LoadStandardCursor(IDC_ARROW));

		//:WARNING - Do not remove
		nFlags |= MK_MBUTTON;
		theDelivery.View.OnMButtonUp(nFlags, point.x, point.y);
	}

	__super::OnMButtonUp(nFlags, point);
}



int WndView2d::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
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



void WndView2d::OnMouseLeave()
{
	theMainFrame.UioManager.OnMouseLeave();
}



void WndView2d::OnMouseMove(UINT nFlags, CPoint point)
{
	if (IsValid() && MousePoint != point) {
		// TEST - CtlStatusBar::ShowCoordinate()
		/*
		double* pValue = Cnt2d::GetCoordinate(ViewId, point.x, point.y);
		theMainFrame.m_statusBar.ShowCoordinate(pValue[0], pValue[1]);
		REMOVE_ARRAY(pValue);
		*/
		theMainFrame.UioManager.OnMouseMove(nFlags, point);
		theDelivery.View.OnMouseMove(nFlags, point.x, point.y);

		//:CHECK - Mouse tracking form UioManager(OnMouseLeave)
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = GetSafeHwnd();

		TrackMouseEvent(&tme);
	}

	MousePoint = point;

	CView::OnMouseMove(nFlags, point);
}



BOOL WndView2d::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	if (ValidRenderer) {
		Painting = true;
		ScreenToClient(&point);

		zDelta *= TheAppOptions.GetBoolean("Environment/Mouse/ReverseWheelDirection") ? -1 : 1;
		theDelivery.View.OnMouseWheel(nFlags, zDelta, point.x, point.y);
	}

	return WndView::OnMouseWheel(nFlags, zDelta, point);
}



void WndView2d::OnPaint()
{
	CPaintDC dc(this);
	CRect rect = GetClientArea();

	if (IsValid()) {
		OutputDebugString(L"OnPaint\n");
		if (Painting == false) {
			Painting = true;
			theDelivery.View.OnPaint();
		}
	}
	else {
		dc.FillSolidRect(rect, (COLORREF)Ctl::EColor::DarkBack);
	}
}



void WndView2d::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		SetFocus();
		SetCapture();

		theDelivery.View.OnRButtonDown(nFlags, point.x, point.y);
	}

	__super::OnRButtonDown(nFlags, point);
}



void WndView2d::OnRButtonUp(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		ReleaseCapture();
		SetCursor(TheApp.LoadStandardCursor(IDC_ARROW));

		//:WARNING - Do not remove
		nFlags |= MK_RBUTTON;
		theDelivery.View.OnRButtonUp(nFlags, point.x, point.y);
	}

	__super::OnRButtonUp(nFlags, point);
}



void WndView2d::OnSize(UINT nType, int cx, int cy)
{
	//:REF - nType: SIZE_MAXIMIZED, ...

	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		ToolBarCtl.AdjustLayout();
		HistoryBarCtl.AdjustLayout();
		theMainFrame.TaskBarCtl.AdjustLayout();
	}

	if (IsValid()) {
		theDelivery.View.OnResize(cx, cy);
	}
}



void WndView2d::OnUpdateCommand(CCmdUI* pCmdUI)
{
}

#undef theDelivery
#undef theMainFrame
