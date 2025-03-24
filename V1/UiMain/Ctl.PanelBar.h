#pragma once

#include "Ctl.h"

class ClsPanelManager;

// Signle instance in MainFrame
//--------------------------------------------------------------------------------------------------

class CtlPanelBar : public CBCGPDockingControlBar
{
public:

	ClsPanelManager* ActivePanelManager = nullptr;

public:

	CtlPanelBar() {}

	~CtlPanelBar() override {}

public:

	bool Initialize(CWnd* pMainFrame);

	void ViewChanged(ClsPanelManager* pManager);

public:

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

