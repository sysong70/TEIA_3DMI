#pragma once

#include "Ctl.h"

class CtlTaskPanel;
class WndView;

//--------------------------------------------------------------------------------------------------

class CtlTaskBar : public CWnd
{
public:

	// Header
	int HeaderHeight = 0;
	CBCGPStatic TitleCtl;
	CBCGPButton CloseCtl;
	// Body
	CtlTaskPanel* TaskPanelCtl = nullptr;
	// Footer
	int FooterHeight = 0;
	CBCGPButton CloseButtonCtl;

public:

	CtlTaskBar() {}

	~CtlTaskBar() override {}

public:

	bool Initialize(CWnd* pMainFrame);

	CSize AdjustLayout();

	WndView* GetView() { return (WndView*)GetParent(); }

	void SetPanel(CtlTaskPanel* pPanel);
	// Delete previous panel and Show/Hide bar
	void Show(WndView* pTargetView);

public:

	void GetArea(CRect& header, CRect& body, CRect& footer);

	void OnClose();

public:

	BOOL PreTranslateMessage(MSG* pMsg) override;

	afx_msg LRESULT OnDPIChangedAfterParent(WPARAM, LPARAM);

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()
};
