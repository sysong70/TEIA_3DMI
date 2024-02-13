#include "stdafx.h"
#include "resource.h"
#include "Window.View3d.h"
#include "Window.Document.h"
#include "Window.MainFrame.h"
#include "Command.Base.h"
#include "Connector.h"
#include "Facility.h"
#include "Facility.AppOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------------

#define PRESET PresetView3d

namespace PresetView3d
{
	int ViewIndex = 0;
	const UINT_PTR ActivateDelayTimer = 1234567;

	enum EPanelId
	{
		TabId = WM_USER,
		ModelTree,
		View,
		Layer,
		Scene,
	};
}

//--------------------------------------------------------------------------------------------------

using namespace Window;

IMPLEMENT_DYNCREATE(View3d, View)

BEGIN_MESSAGE_MAP(View3d, View)
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
	ON_MESSAGE((UINT)EUserMessage::OnSignal, OnSignal)
END_MESSAGE_MAP()



Window::View3d::View3d()
	: View()
{
	m_eType = EType::View3d;

	m_delivery.SetSender(Connector3d::GetSender());
	m_delivery.ViewId = m_nViewId;
	m_delivery.view.OnConstruct();
}



Window::View3d::~View3d()
{
	m_modelPanel.DestroyWindow();
	m_viewPanel.DestroyWindow();
	m_layerPanel.DestroyWindow();
	m_scenePanel.DestroyWindow();
}



void Window::View3d::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;
	Signal::Target target = (Signal::Target)data.GetInteger(SKW_TARGET);

	if (target == Signal::Target::ModelPanel) {
		m_modelPanel.ReceiveSignal(pData);
		return;
	}
	else if (target == Signal::Target::View) {
		Signal::View::Action action = (Signal::View::Action)data.GetInteger(SKW_ACTION);

		switch (action) {
		case Signal::View::Action::SetValidation:
			m_bRenderer = data.GetBoolean(SKW_VALID);
			if (m_bRenderer) {
				CRect rect = GetClientArea();
				m_delivery.view.OnPaint(rect.left, rect.top, rect.right, rect.bottom);

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
	else {
		DEBUG_STOP;
	}

	REMOVE_POINTER(pData);
}



void Window::View3d::OnCommand(UINT id)
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

	Facility::CommandIndexer::CommandInfo& data = TheCommandIndexer.Get(id);

	if (data.Function != nullptr) {
		Command::Base* pCommand = (Command::Base*)data.Function;
		if (pCommand->IsRunOnlyOnce() == false) {
			m_pActiveCommand = pCommand;
		}

		pCommand->Run(this);
	}
	else {
		switch (data.Type) {
		case Facility::CommandIndexer::Popup:
			break;

		case Facility::CommandIndexer::Unknown:
			DEBUG_STOP;
			break;

		case Facility::CommandIndexer::ListItem:
		case Facility::CommandIndexer::Check: //:TEMP
		default:
			m_delivery.view.OnCommand(id);
			break;
		}
	}
}



void Window::View3d::OnUpdateCommand(CCmdUI* pCmdUI)
{
	//for (auto id : m_checkedCommands) {
	//	if (id == pCmdUI->m_nID) {
	//		pCmdUI->SetCheck(TRUE);
	//		return;
	//	}
	//}

	//pCmdUI->SetCheck(FALSE);
}



BOOL Window::View3d::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	if (m_bRenderer) {
		CRect rect;
		GetWindowRect(rect);

		zDelta *= TheAppOptions.GetBoolean("Environment/Mouse/ReverseWheelDirection") ? -1 : 1;
		m_delivery.view.OnMouseWheel(nFlags, zDelta, point.x, point.y, rect.left, rect.top, rect.right, rect.bottom);
	}

	return __super::OnMouseWheel(nFlags, zDelta, point);
}



void Window::View3d::CreateToolBar()
{
	__super::CreateToolBar();

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
	});
}



void Window::View3d::CreatePanelTabs()
{
	__super::CreatePanelTabs();

	m_modelPanel.Initialize(&m_tabs, this, PRESET::ModelTree);
	m_viewPanel.Initialize(&m_tabs, this, PRESET::View);
	m_layerPanel.Initialize(&m_tabs, this, PRESET::Layer);
	m_scenePanel.Initialize(&m_tabs, this, PRESET::Scene);

	int image = 0;

	m_tabs.SetImageList({ HOME_3D_CMD_Panels_Model, HOME_3D_CMD_Panels_View, HOME_3D_CMD_Panels_Layer, HOME_3D_CMD_Panels_Scene, },
		Control::ImageSize());
	m_tabs.AddTab(&m_modelPanel, Facility::GetTitle(HOME_3D_CMD_Panels_Model), image++);
	m_tabs.AddTab(&m_viewPanel, Facility::GetTitle(HOME_3D_CMD_Panels_View), image++);
	m_tabs.AddTab(&m_layerPanel, Facility::GetTitle(HOME_3D_CMD_Panels_Layer), image++);
	m_tabs.AddTab(&m_scenePanel, Facility::GetTitle(HOME_3D_CMD_Panels_Scene), image++);
	m_tabs.SetActiveTab(0);
}

#undef PRESET
