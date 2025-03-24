#include "stdafx.h"

#include "Ast.h"
#include "Cls.PanelManager.h"
#include "Wnd.View.h"

//**************************************************************************************************

bool ClsPanelManager::Initialize(WndView* pView, UINT id)
{
	ViewWnd = pView;

	if (PanelTabs.Create(CBCGPTabWnd::STYLE_3D, {}, pView, WM_USER, CBCGPTabWnd::LOCATION_TOP) == FALSE) {
		RETURN_FALSE;
	}

	PanelTabs.SetTabHeight(Ctl::TabHeight());
	PanelTabs.SetLocation(CBCGPTabWnd::LOCATION_TOP);
	PanelTabs.SetIconLocation(CBCGPTabWnd::TAB_ICON_LEFT);

	return true;
}



ClsPanelManager* ClsPanelManager::Activate(CWnd* pParent, CSize size)
{
	PanelTabs.ShowWindow(SW_HIDE);
	PanelTabs.SetParent(pParent);
	PanelTabs.SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);
	PanelTabs.ShowWindow(SW_SHOW);

	return this;
}



void ClsPanelManager::ActivatePanel(int id)
{
	BOOL result = PanelTabs.SetActiveTab(id);
	ASSERT(result);
}



void ClsPanelManager::OnSize(UINT nType, int cx, int cy)
{
	if (PanelTabs.GetSafeHwnd() != nullptr) {
		PanelTabs.SetWindowPos(NULL, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

//**************************************************************************************************

bool ClsPanelManager2d::Initialize(WndView* pView, UINT id)
{
	__super::Initialize(pView, id);

	LayerPanel.Initialize(&PanelTabs, pView, WM_USER);

	PanelTabs.SetImageList({ HOME_2D_CMD_Panels_Layer }, Ctl::ImageSize());
	PanelTabs.AddTab(&LayerPanel, Ast::GetTitle(HOME_2D_CMD_Panels_Layer), 0);
	PanelTabs.SetActiveTab(0);

	return false;
}

//**************************************************************************************************

bool ClsPanelManager3d::Initialize(WndView* pView, UINT id)
{
	__super::Initialize(pView, id);

	LayerPanel.Initialize(&PanelTabs, pView, (UINT)Id::Layer);
	ModelPanel.Initialize(&PanelTabs, pView, (UINT)Id::ModelTree);
	ScenePanel.Initialize(&PanelTabs, pView, (UINT)Id::Scene);
	ViewPanel.Initialize(&PanelTabs, pView, (UINT)Id::View);

	int image = 0;

	PanelTabs.SetImageList(
		{ HOME_3D_CMD_Panels_Model, HOME_3D_CMD_Panels_View, HOME_3D_CMD_Panels_Layer, HOME_3D_CMD_Panels_Scene, },
		Ctl::ImageSize()
	);
	PanelTabs.AddTab(&ModelPanel, Ast::GetTitle(HOME_3D_CMD_Panels_Model), image++);
	PanelTabs.AddTab(&ViewPanel, Ast::GetTitle(HOME_3D_CMD_Panels_View), image++);
	PanelTabs.AddTab(&LayerPanel, Ast::GetTitle(HOME_3D_CMD_Panels_Layer), image++);
	PanelTabs.AddTab(&ScenePanel, Ast::GetTitle(HOME_3D_CMD_Panels_Scene), image++);
	PanelTabs.SetActiveTab(0);

    return true;
}
