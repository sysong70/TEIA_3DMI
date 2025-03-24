#pragma once

#include "Ctl.h"
#include "Ctl.PropList.h"

class ClsUserIoManager;
class WndView;

//--------------------------------------------------------------------------------------------------

class CtlTaskIoBar : public CWnd
{
public:

	CBCGPStatic TitleCtl;
	CtlPropList PropListCtl;

	ClsUserIoManager& UioManager;
	Json::Object* DesignData = nullptr;
	bool Initialized = false;

public:

	CtlTaskIoBar(ClsUserIoManager& manager)
		: UioManager(manager)
	{}

	~CtlTaskIoBar() override {}

public:

	bool Initialize(CWnd* pParentWnd);

	CSize AdjustLayout();

	void Show(WndView* pTargetView);

public:

	// Not window event
	bool OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	// Not window event
	bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	BOOL PreTranslateMessage(MSG* pMsg) override;

	afx_msg LRESULT OnCommandClicked(WPARAM wp, LPARAM lp);

	afx_msg LRESULT OnDPIChangedAfterParent(WPARAM, LPARAM);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

public:

	bool SetCommand() { return false; }

	bool SetPrompt();

	bool SetEcho(const CString& value) { return false; }

	bool SetError(const CString& value) { return false; }

	bool StandbyCommand(const CString& prompt);
};
