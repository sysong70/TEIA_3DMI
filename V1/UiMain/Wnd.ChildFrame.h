#pragma once

#include "Wnd.h"

//--------------------------------------------------------------------------------------------------

class WndChildFrame : public CBCGPMDIChildWnd
{
public:

	WndChildFrame() {}

	~WndChildFrame() override {}

public:

	void ActivateFrame(int nCmdShow) override;

	BOOL PreCreateWindow(CREATESTRUCT& cs) override;

#ifdef _DEBUG
	void AssertValid() const override;

	void Dump(CDumpContext& dc) const override;
#endif

public:

	DECLARE_DYNCREATE(WndChildFrame)
	DECLARE_MESSAGE_MAP()
};
