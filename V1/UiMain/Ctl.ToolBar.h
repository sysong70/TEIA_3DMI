#pragma once

#include "Ctl.h"

//--------------------------------------------------------------------------------------------------

class CtlToolBar : public CWnd
{
public:

	enum class EIcon
	{
		Small,
		Medium,
		Large,
	};

	Ctl::EPivot Pivot = Ctl::EPivot::TopCenter;
	bool ExpandSize = false;
	bool Transparent = false;

	std::vector<CBCGPButton*> Buttons;

	CSize ButtonSize = CSize(28, 28);
	CSize ButtonMargin = CSize(2, 2);
	CSize ImageSize = CSize(24, 24);
	CSize SeperatorMargin = CSize(6, 6);
	CSize ToolBarPadding = CSize(3, 3);

public:

	CtlToolBar() {}

	~CtlToolBar() override;

	bool Initialize(CWnd* pParentWnd, UINT id = WM_USER, const RECT& rect = {});

	void SetPivot(Ctl::EPivot pivot, bool expandSize = true);

public: // CBCGPButton

	CBCGPButton* AddButton(UINT id, bool menu = false);

	void AddButtons(const Ctl::ResourceIds& ids);

	void AddSeperator();

public: // CBCGPButton (toggle)

	CBCGPButton* AddToggle(UINT id, bool checked = false);

	bool GetCheck(UINT id);

	void SetCheck(UINT id, bool value, bool uncheckOthers = true);

	void SetUncheckOthers(UINT id);

public: // ButtonWithMenu

	CBCGPButton* AddButtonWithMenu(UINT id, const Ctl::ResourceIds& menus);

	void CheckMenu(UINT id, UINT menuId, bool radioType = false);

	UINT GetMenuResult(UINT id);

public:

	CSize AdjustLayout();

	CPoint AdjustLocation(CSize size);

	CBCGPButton* GetButton(UINT id);

	void SetSize(CSize buttonSize, CSize buttonMargin, CSize imageSize, CSize seperatorMargin, CSize toolBarPadding);
	// image size, not button size
	void SetSize(EIcon size);

public:

	afx_msg void OnCommand(UINT id);

	afx_msg LRESULT OnDPIChangedAfterParent(WPARAM, LPARAM);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()

public:

	// return CBCGPButton or ButtonWithMenu
	CBCGPButton* CreateButton(UINT id, bool menu, bool toggle = false, bool checked = false);

	bool IsHorizontal();

	CSize GetButtonSize();

	CSize GetButtonMargin();

	CSize GetImageSize();

	CSize GetSeperatorMargin();

	CSize GetPadding();
};

//--------------------------------------------------------------------------------------------------

class CtlHistoryBar : public CtlToolBar
{
	int MaxCount = 10;

public:

	CtlHistoryBar() {}

	~CtlHistoryBar() override {}

public:

	void PushButton(UINT id);

	void RemoveButton(UINT id);
};
