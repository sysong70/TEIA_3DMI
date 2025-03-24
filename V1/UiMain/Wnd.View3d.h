#pragma once

#include "Cls.PanelManager.h"
#include "Wnd.View.h"

//--------------------------------------------------------------------------------------------------

class WndView3d : public WndView
{
public:

	ClsPanelManager3d PanelManager;

public:

	WndView3d();

	~WndView3d() override;

	void Activate(bool value) override;

	void CancelCommand() override;

	void CreateToolBar() override;

	Signal::Delivery* GetDelivery3d() override;

	ClsPanelManager* GetPanelManager() override { return &PanelManager; }

	bool IsValid() override;

	void ReceiveSignal(Json::Object* pData) override;

public:

	// WARNING - abstract
	void OnDraw(CDC* pDC) override {}

	void OnInitialUpdate() override;

	BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnCommand(UINT id);
	//:WARNING - Do not remove background
	afx_msg BOOL OnEraseBkgnd(CDC* pDC) { return TRUE; }

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

	afx_msg void OnPaint();

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnUpdateCommand(CCmdUI* pCmdUI);

	DECLARE_DYNCREATE(WndView3d)
	DECLARE_MESSAGE_MAP()
};
