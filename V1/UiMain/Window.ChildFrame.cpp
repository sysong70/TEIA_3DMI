#include "stdafx.h"
#include "Window.ChildFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace Window;

IMPLEMENT_DYNCREATE(ChildFrame, CBCGPMDIChildWnd)

BEGIN_MESSAGE_MAP(ChildFrame, CBCGPMDIChildWnd)
END_MESSAGE_MAP()




Window::ChildFrame::ChildFrame()
{
	// TODO: add member initialization code here
}



Window::ChildFrame::~ChildFrame()
{
}



void Window::ChildFrame::ActivateFrame(int nCmdShow)
{
	__super::ActivateFrame(SW_SHOWMAXIMIZED);
}



BOOL Window::ChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	if (__super::PreCreateWindow(cs) == FALSE) {
		RETURN_FALSE;
	}

	return TRUE;
}

#ifdef _DEBUG

void Window::ChildFrame::AssertValid() const
{
	__super::AssertValid();
}



void Window::ChildFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIChildWnd::Dump(dc);
}

#endif //_DEBUG
