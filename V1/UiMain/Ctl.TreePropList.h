#pragma once

#include "Ctl.h"
#include "Ctl.PropList.h"
#include "Ctl.TreeCtrlEx.h"

//--------------------------------------------------------------------------------------------------

using OnPropertyChangedFunc = void (CWnd::*)(Json::Value& target, CBCGPProp& source);

//--------------------------------------------------------------------------------------------------

class CtlTreePropList : public CWnd
{
public:

	CString Dictionary;
	CtlTreeEx TreeCtl;
	CtlPropList PropListCtl;

	CWnd* PropertyChangedHandler = nullptr;

	bool Modified = false;
	Json::Object* DesignData = nullptr;
	Json::Object* ValueData = nullptr;

public:

	CtlTreePropList();

	~CtlTreePropList() override;

public:

	bool Initialize(CWnd* pParentWnd, UINT id = WM_USER, const RECT& rect = {});

	bool InitializeDesign(Json::Object& design);

	bool InitializeValue(Json::Object& data);

	void RefreshData();

	void OnPropertyChangedHandler(CWnd* pTarget) { PropertyChangedHandler = pTarget; }

public:

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

public:

	void ChangePropList(HTREEITEM pItem);
};

