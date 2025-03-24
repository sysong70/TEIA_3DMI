#pragma once

#include "Ctl.h"

//--------------------------------------------------------------------------------------------------

class CtlTreeEx : public CBCGPTreeCtrlEx
{
public:

	bool Initialized = false;

public:

	CtlTreeEx();

	~CtlTreeEx() override;

public:

	bool Initialize(CWnd* pParentWnd, UINT id = WM_USER, const RECT& rect = {});

	bool InitializeDesign(Json::Array& design);

	void GetAncestor(HTREEITEM pItem, std::vector<HTREEITEM>& parent);
	// From GetItemData(pItem)
	CString GetItemNamePath(HTREEITEM pItem);

public:

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()

public:

	HTREEITEM CreateItem(Json::Object& design, HTREEITEM pParent);

	void CreateItem(Json::Array& design, HTREEITEM pParent);
};
