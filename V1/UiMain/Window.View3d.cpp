#include "stdafx.h"
#include "resource.h"
#include "Window.Document.h"
#include "Window.MainFrame.h"
#include "Window.View3d.h"
#include "Connector.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



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

	CSize TabImageSize()
	{
		return globalUtils.ScaleByDPI(CSize(24, 24));
	}
}



using namespace Window;

IMPLEMENT_DYNCREATE(View3d, View)

BEGIN_MESSAGE_MAP(View3d, View)
	ON_WM_MOUSEWHEEL()
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
}



void Window::View3d::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;
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

	REMOVE_POINTER(pData);
}


BOOL Window::View3d::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	if (m_bRenderer) {
		CRect rect;
		GetWindowRect(rect);
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

	m_modelPanel.Initialize(&m_tabs, PRESET::ModelTree);
	m_viewPanel.Initialize(&m_tabs, PRESET::View);
	m_layerPanel.Initialize(&m_tabs, PRESET::Layer);
	m_scenePanel.Initialize(&m_tabs, PRESET::Scene);

	int image = 0;

	m_tabs.SetImageList({ HOME_3D_CMD_Panels_Model, HOME_3D_CMD_Panels_View, HOME_3D_CMD_Panels_Layer, HOME_3D_CMD_Panels_Scene, },
		PRESET::TabImageSize());
	m_tabs.AddTab(&m_modelPanel, Facility::GetTitle(HOME_3D_CMD_Panels_Model), image++);
	m_tabs.AddTab(&m_viewPanel, Facility::GetTitle(HOME_3D_CMD_Panels_View), image++);
	m_tabs.AddTab(&m_layerPanel, Facility::GetTitle(HOME_3D_CMD_Panels_Layer), image++);
	m_tabs.AddTab(&m_scenePanel, Facility::GetTitle(HOME_3D_CMD_Panels_Scene), image++);
	m_tabs.SetActiveTab(0);
}

#undef PRESET
