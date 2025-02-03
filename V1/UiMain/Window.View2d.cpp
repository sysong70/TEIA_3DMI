#include "stdafx.h"
#include "resource.h"
#include "Window.View2d.h"
#include "Window.Document.h"
#include "Window.MainFrame.h"
#include "Command.Base.h"
#include "Connector.h"
#include "Facility.AppOptions.h"
#include "Facility.CommandIndexer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define PRESET PresetView2d

namespace PresetView2d
{
	enum EPanelId
	{
		TabId = WM_USER,
		Layer,
	};
}

//**************************************************************************************************

using namespace Window;

IMPLEMENT_DYNCREATE(View2d, CView)

BEGIN_MESSAGE_MAP(View2d, CView)
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

	// CHECK - set range
	ON_COMMAND_RANGE(CONTEXT_2D_START, CONTEXT_2D_END, OnContextCommand)
	ON_COMMAND_RANGE(COMMAND_START, COMMAND_END, OnCommand)
	ON_MESSAGE((UINT)EUserMessage::OnSignal, OnSignal)
END_MESSAGE_MAP()



Window::View2d::View2d()
	: View()
{
	m_eType = EViewType::View2d;

	GetDelivery().view.OnConstruct();
}



Window::View2d::~View2d()
{
	m_layerPanel.DestroyWindow();
}



Signal::Delivery& Window::View2d::GetDelivery()
{
	return Connector2d::GetInstance(m_nViewId);
}



void Window::View2d::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;
	Signal::Target target = (Signal::Target)data.GetInteger(SKW_TARGET);

	if (target == Signal::Target::LayerPanel) {
	}
	else if (target == Signal::Target::View) {
		Signal::View::Action action = (Signal::View::Action)data.GetInteger(SKW_ACTION);

		switch (action) {
			case Signal::View::Action::SetValidation:
				m_bRenderer = data.GetBoolean(SKW_VALID);
				if (m_bRenderer) {
					CRect rect = GetClientArea();
					GetDelivery().view.OnPaint(rect.left, rect.top, rect.right, rect.bottom);

					if (GetMainFrame().HasNextFile()) {
						GetMainFrame().PostMessage((UINT)EUserMessage::OnNextFileOpen);
					}
					else {
						SendMessage(WM_ACTIVATE, (WPARAM)WA_ACTIVE);
					}
				}
				else {
					GetDocument()->OnCloseDocument();
				}
				break;

			case Signal::View::Action::SetContextMenu:
				SetContextMenu(pData);
				break;

			case Signal::View::Action::ShowContextMenu:
				ShowContextMenu(pData);
				break;

			default:
				DEBUG_STOP;
				break;
		}
	}
	else if (target == Signal::Target::TaskBar) {
		ASSERT(data.GetInteger(SKW_VIEWID, -1) == m_nViewId);

		if (m_pActiveCommand != nullptr) {
			m_pActiveCommand->ReceiveSignal(pData);
			return;
		}
		else {
			DEBUG_STOP;
		}
	}

	REMOVE_POINTER(pData);
}




bool Window::View2d::SetContextMenu(Json::Object* pData)
{
	DEBUG_VALID(pData);
	Json::Object& data = *pData;
	CMenu& menu = GetMainFrame().GetContextMenu(true);

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
				osnapMenu.AppendMenu(MF_STRING, subId, Facility::GetTitle(subId));
			}

			// WARNING - osnapMenu.Detach()
			menu.AppendMenu(MF_STRING | MF_POPUP, (UINT_PTR)osnapMenu.Detach(), Facility::GetTitle(id));
		}
		else {
			menu.AppendMenu(MF_STRING, id, Facility::GetTitle(id));
		}
	}

	return true;
}



void Window::View2d::ShowContextMenu(Json::Object* pData)
{
	SetContextMenu(pData);

	CPoint point;
	::GetCursorPos(&point);

	CMenu& menu = GetMainFrame().GetContextMenu(false);
	UINT id = TheApplication.GetContextMenuManager()->TrackPopupMenu(menu, point.x, point.y, this);
	if (id > 0) {
		OnContextCommand(id);
	}
}



void Window::View2d::OnCommand(UINT id)
{
	// global post process

	if (GetMainFrame().HasCommandHandeler(id)) {
		GetMainFrame().OnCommand(id);
		return;
	}

	switch (id) {
	case HOME_2D_CMD_Panels_Model:
	case HOME_2D_CMD_Panels_View:
	case HOME_2D_CMD_Panels_Layer:
		DEBUG_STOP;
		return;

	case CUSTOM_3D_CMD_KEN_Test3:
		GetDelivery().userIO.OnInput(L"LINE");
		return;

	default:
		if (m_bRenderer == false) {
			return;
		}
	}

	Facility::CommandIndexer::CommandInfo* pInfo = &TheCommandIndexer.Get(id);
	DEBUG_VALID(pInfo);

	if (pInfo != nullptr && pInfo->ChildId >= 0) {
		id = pInfo->ChildId;
		// WARNING - replace info (do not reference valuable)
		pInfo = &TheCommandIndexer.Get(id);
		DEBUG_VALID(pInfo);
	}

	// TEST
	//m_historyBar.PushButton(id);
	CancelCommand();

	if (pInfo->Function != nullptr) {
		Command::Base* pCommand = pInfo->Function;
		if (pCommand->IsRunOnlyOnce() == false) {
			m_pActiveCommand = pCommand;
		}

		pCommand->Run(this);
	}
	else {
		switch (pInfo->Type) {
			case Facility::CommandIndexer::Popup:
				break;

			case Facility::CommandIndexer::Unknown:
				DEBUG_STOP;
				break;

			case Facility::CommandIndexer::ListItem:
			case Facility::CommandIndexer::Check: // TEMP
			default:
				GetDelivery().view.OnCommand(id);
				break;
		}
	}
}



void Window::View2d::OnContextCommand(UINT id)
{
	GetDelivery().view.OnContextCommand(id);
}



void Window::View2d::OnMouseMove(UINT nFlags, CPoint point)
{
	if (IsValid()) {
		// TSET
		double* pValue = Connector2d::GetCoordinate(m_nViewId, point.x, point.y);
		GetMainFrame().m_statusBar.ShowCoordinate(pValue[0], pValue[1]);
		REMOVE_ARRAY(pValue);

		GetDelivery().view.OnMouseMove(nFlags, point.x, point.y);
	}

	CView::OnMouseMove(nFlags, point);
}



BOOL Window::View2d::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	if (m_bRenderer) {
		ScreenToClient(&point);

		zDelta *= TheAppOptions.GetBoolean("Environment/Mouse/ReverseWheelDirection") ? -1 : 1;
		GetDelivery().view.OnMouseWheel(nFlags, zDelta, point.x, point.y);
	}

	return __super::OnMouseWheel(nFlags, zDelta, point);
}



void Window::View2d::CreateCommandPrompt()
{
}



void Window::View2d::CreateToolBar()
{
	__super::CreateToolBar();

	m_toolBar.AddButtons({
		HOME_2D_CMD_Pan,
		HOME_2D_CMD_Zoom_Fit,
	});
}



void Window::View2d::CreatePanelTabs()
{
	__super::CreatePanelTabs();

	m_layerPanel.Initialize(&m_tabs, this, PRESET::Layer);

	m_tabs.SetImageList({ HOME_3D_CMD_Panels_Layer }, Control::ImageSize());
	m_tabs.AddTab(&m_layerPanel, Facility::GetTitle(HOME_3D_CMD_Panels_Layer), 0);
	m_tabs.SetActiveTab(0);
}

#undef PRESET
