#pragma once

#include "Dio.h"

//--------------------------------------------------------------------------------------------------

class DioItem
{
public:

	Dio::EState State = Dio::EState::Unchanged;
	int AcceptMask = Dio::eAcceptAll;
	int SkipMask = 0;
	int EnterMask = 0;

	bool Editable = false;
	bool Nullable = false;
	bool Relative = true;
	bool Angular = true;

	CString Text;

	virtual void SetValue(const CString& value) {}

	virtual CString GetValue() { return Text; }
};

//--------------------------------------------------------------------------------------------------

class CtlDioEdit : public DioItem, public CEdit
{
public:

	~CtlDioEdit() override;

	void SetValue(const CString& value) override;

protected:

	//afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	//afx_msg void OnContextMenu(CWnd*, CPoint point);

	//afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	//afx_msg void OnPaint();

	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	//afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	//afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	//afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	//afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	//afx_msg void OnEnChange();

	//DECLARE_MESSAGE_MAP()
};

//--------------------------------------------------------------------------------------------------

class CtlDioLabel : public DioItem, public CStatic
{
};

//--------------------------------------------------------------------------------------------------
// Popup Window

class WndDioItemPopup : public CWnd
{
	DioItem* ItemPtr = nullptr;

public:

	~WndDioItemPopup() override;

	BOOL Create(Dio::EControlId id);

	virtual void SetValue(const CString& value) {}

	virtual CString GetValue() { return ItemPtr->Text; }

protected:

	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
};
