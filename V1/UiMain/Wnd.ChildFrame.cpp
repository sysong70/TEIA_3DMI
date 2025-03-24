#include "stdafx.h"

#include "Wnd.ChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

IMPLEMENT_DYNCREATE(WndChildFrame, CBCGPMDIChildWnd)

BEGIN_MESSAGE_MAP(WndChildFrame, CBCGPMDIChildWnd)
END_MESSAGE_MAP()



void WndChildFrame::ActivateFrame(int nCmdShow)
{
	__super::ActivateFrame(SW_SHOWMAXIMIZED);
}



BOOL WndChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if (__super::PreCreateWindow(cs) == FALSE) {
		RETURN_FALSE;
	}

	return TRUE;
}

#ifdef _DEBUG

void WndChildFrame::AssertValid() const
{
	__super::AssertValid();
}



void WndChildFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIChildWnd::Dump(dc);
}

#endif //_DEBUG
