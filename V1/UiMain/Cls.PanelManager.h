#pragma once

#include "Ctl.TabWnd.h"
#include "Ctl.Panel2dLayer.h"
#include "Ctl.Panel3dLayer.h"
#include "Ctl.Panel3dModel.h"
#include "Ctl.Panel3dScene.h"
#include "Ctl.Panel3dView.h"

class WndView;

// Instance by View
//--------------------------------------------------------------------------------------------------

class ClsPanelManager
{
public:

	WndView* ViewWnd = nullptr;
	CtlTabWnd PanelTabs;

	virtual bool Initialize(WndView* pView, UINT id);

public:

	// Activate this
	ClsPanelManager* Activate(CWnd* pParent, CSize size);
	// Activate panel
	void ActivatePanel(int id);

	void OnSize(UINT nType, int cx, int cy);
};

//--------------------------------------------------------------------------------------------------

class ClsPanelManager2d : public ClsPanelManager
{
public:

	CtlPanel2dLayer	LayerPanel;

	bool Initialize(WndView* pView, UINT id) override;
};

//--------------------------------------------------------------------------------------------------

class ClsPanelManager3d : public ClsPanelManager
{
public:

	enum class Id
	{
		Layer = WM_USER,
		ModelTree,
		Scene,
		View,
	};

	CtlPanel3dLayer	LayerPanel;
	CtlPanel3dModel	ModelPanel;
	CtlPanel3dScene	ScenePanel;
	CtlPanel3dView	ViewPanel;

	bool Initialize(WndView* pView, UINT id) override;
};
