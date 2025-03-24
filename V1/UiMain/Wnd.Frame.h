#pragma once

#include "Wnd.h"

//--------------------------------------------------------------------------------------------------

class WndFrame : public CWnd
{
public:

	WindowParams WndParams;

public:

	WndFrame() {}

	virtual bool Create();

	virtual bool Create(const WindowParams& params);

	virtual CSize AdjustLayout() { return {}; }

	virtual CSize SizeToChildren() { return {}; }

public:

	BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnPaint();

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);

	DECLARE_MESSAGE_MAP();
};

