#pragma once

#include "Dio.h"

class WndView;

//--------------------------------------------------------------------------------------------------

class CtlDynamicEdit : public CBCGPEdit
{
public:

	CtlDynamicEdit();

	~CtlDynamicEdit() override;

public:

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnContextMenu(CWnd*, CPoint point) {}

	afx_msg BOOL OnEraseBkgnd(CDC* pDC) {}

	afx_msg void OnPaint() {}

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {}

	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {}

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point) {}

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point) {}

	afx_msg void OnMButtonDown(UINT nFlags, CPoint point) {}

	afx_msg void OnMButtonUp(UINT nFlags, CPoint point) {}

	afx_msg void OnMouseMove(UINT nFlags, CPoint point) {}

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point) {}

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point) {}

	afx_msg void OnEnChange() {}

	DECLARE_MESSAGE_MAP()
};

//--------------------------------------------------------------------------------------------------

class CtlFitLabel : public CStatic
{
public:

	CtlFitLabel() {}

	~CtlFitLabel() {}

public:

	CSize SetText(const CString& value);

	CSize CalcSize();
};

//--------------------------------------------------------------------------------------------------

class CtlDioWrapper : public CWnd
{
public:

	CWnd* Child = nullptr;
	DynamicIoParams Params;

public:

	CtlDioWrapper()
		: CWnd()
	{}

	~CtlDioWrapper() override;

	bool Initialize(CWnd* pParentWnd, const RECT& rect, Dio::EControlId id);

	CSize SetText(const CString& value, bool select = false);

	CSize CalcSize();

public:

	BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	bool OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
};

