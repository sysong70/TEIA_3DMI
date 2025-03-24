#include "stdafx.h"

#include "Cmd.Base.h"
#include "Cnt.h"
#include "Wnd.Application.h"
#include "Wnd.Document.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	int ViewIndex = 0;
}

//**************************************************************************************************

IMPLEMENT_DYNCREATE(WndView, CView)

BEGIN_MESSAGE_MAP(WndView, CView)
END_MESSAGE_MAP()



void WndView::CreateHistoryBar(Ctl::EPivot pivot)
{
	HistoryBarCtl.SetPivot(pivot, false);
	HistoryBarCtl.Initialize(this);
}



void WndView::CreateToolBar()
{
	ToolBarCtl.Initialize(this);
}

//--------------------------------------------------------------------------------------------------

WndView::WndView()
{
	ViewId = ViewIndex++;
}



WndView::~WndView()
{
	//:WARNING - Change after OnDestruct()
	ViewId = -1;
	TheApp.GetMainFrame().ViewChanged(WM_DESTROY, this);
}



CRect WndView::GetClientArea()
{
	CRect rect;
	GetClientRect(rect);

	return rect;
}



CSize WndView::GetClientSize()
{
	return GetClientArea().Size();
}



WndDocument* WndView::GetDocument() const
{
	return (WndDocument*)m_pDocument;
}

//--------------------------------------------------------------------------------------------------

void WndView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	__super::OnActivateView(bActivate, pActivateView, pDeactiveView);

	Activate(bActivate);
}

#ifdef _DEBUG

void WndView::AssertValid() const
{
	__super::AssertValid();
}



void WndView::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG

LRESULT WndView::OnSignal(WPARAM wp, LPARAM lp)
{
	ReceiveSignal((Json::Object*)wp);

	return 0;
}
