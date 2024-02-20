#include "stdafx.h"
#include "resource.h"
#include "Window.View2d.h"
#include "Window.Document.h"
#include "Window.MainFrame.h"
#include "Connector.h"
#include "Facility.h"
#include "Facility.AppOptions.h"

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

	ON_COMMAND_RANGE(COMMAND_START, COMMAND_END, OnCommand)
	ON_MESSAGE((UINT)EUserMessage::OnSignal, OnSignal)
END_MESSAGE_MAP()



Window::View2d::View2d()
	: View()
{
	m_eType = EType::View2d;

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
	Signal::View::Action action = (Signal::View::Action)data.GetInteger(SKW_ACTION);

	switch (action) {
	case Signal::View::Action::SetValidation:
		m_bRenderer = data.GetBoolean(SKW_VALID);
		if (m_bRenderer) {
			//CSize client = GetClientSize();
			//m_delivery.view.OnResize(client.cx, client.cy);
			//m_delivery.view.OnPaint();

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

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}



void Window::View2d::OnCommand(UINT id)
{
	// global post process

	switch (id) {
	case FILE_3D_CMD_New:
	case FILE_3D_CMD_Open:
	case FILE_3D_CMD_Options:
	case HOME_3D_CMD_Window_Cascade:
	case HOME_3D_CMD_Window_TileHorizontal:
	case HOME_3D_CMD_Window_TileVertical:
		GetMainFrame().OnCommand(id);
		return;

	default:
		DEBUG_RETURN;
	}
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



void Window::View2d::CreateToolBar()
{
	__super::CreateToolBar();

	m_toolBar.AddButtons({
		HOME_3D_CMD_Pan,
		HOME_3D_LST_Zoom,
		0,
		HOME_3D_LST_Select,
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
