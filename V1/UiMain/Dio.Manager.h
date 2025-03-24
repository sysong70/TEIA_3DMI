#pragma once

#include "Dio.h"

//--------------------------------------------------------------------------------------------------

class DioContainer
{
protected:

	friend class WndDynamicInput;

	CtlDioEdit LengthCtl;
	CtlDioEdit RadiusCtl;

	CtlDioEdit PromptCtl;
	CtlDioEdit OSnapCtl;

	CtlDioEdit CoordXCtl;
	CtlDioEdit CoordYCtl;
	CtlDioEdit CoordZCtl;
	using Controls = std::vector<DioItem*>;

	Controls ModifiedItems;

protected:

	void Initialize(CWnd* pParent);

public:

	void Clear() { ModifiedItems.clear(); }

	DioItem* Get(Dio::EControlId id);

	CWnd* GetWindow(Dio::EControlId id);

	CString GetValue(Dio::EControlId id);

	void SetValue(Dio::EControlId id, const CString& value);

	void Show(Dio::EControlId id, bool show);
};

//--------------------------------------------------------------------------------------------------

class DioManager
{
protected:

	Window::View* ViewPtr = nullptr;
	DioContainer Container;

	CPoint LastPoint;
	CPoint CurrentPoint;
	bool Activated = false;

	CString Prompt;
	CString Input;
	WStringArray Inputs;

public:

	virtual ~WndDynamicInput();

	bool Initialize(CWnd* pParent);

	void Show(bool show);

	void ViewChanged(Window::View* pView);

public:

	virtual void AdjustLayout() {}

	virtual void SetInputs(Json::Object* pData);

public:

	BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnPaint();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

protected:

	Window::View* GetView();
};
