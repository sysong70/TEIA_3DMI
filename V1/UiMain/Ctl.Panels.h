#pragma once

#include "Ctl.h"
#include "Ctl.ToolBar.h"

class WndView;

//--------------------------------------------------------------------------------------------------

class CtlPanel : public CWnd
{
public:

	WndView* ViewWnd = nullptr;

	CtlToolBar ToolBarCtl;
	int HeaderHeight = 0;
	int FooterHeight = 0;
	CSize OldSize; // OnSize()

public:

	CtlPanel() {}

	~CtlPanel() override { DestroyWindow(); }

	bool Initialize(CWnd* pParentWnd, WndView* pView, UINT id);

public:

	virtual void AdjustLayout(int cx, int cy) { ToolBarCtl.AdjustLayout(); }

	virtual void ConstructBody() {}

	virtual int ConstructFooter(int cx) { return FooterHeight = 0; }
	// Construct toolbar
	virtual int ConstructHeader(int cx) { return HeaderHeight = 0; }
	// exclude margin
	virtual CRect GetBodyRect();
	// include margin
	virtual CSize GetBodySize();

	virtual void ReceiveSignal(Json::Object* pData) { DEBUG_STOP; }

public:

	afx_msg LRESULT OnDPIChangedAfterParent(WPARAM wp, LPARAM lp);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

//--------------------------------------------------------------------------------------------------

class CtlPanel3d : public CtlPanel
{
public:

	~CtlPanel3d() override {}
};

//--------------------------------------------------------------------------------------------------

class CtlPanel2d : public CtlPanel
{
public:

	~CtlPanel2d() override {}
};
