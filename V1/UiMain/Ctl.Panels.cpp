#include "stdafx.h"

#include "Ctl.Panels.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlPanel, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
END_MESSAGE_MAP()



bool CtlPanel::Initialize(CWnd* pParentWnd, WndView* pView, UINT id)
{
	ViewWnd = pView;

	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, {}, pParentWnd, id) == FALSE) {
		RETURN_FALSE;
	}

	CRect rect;
	pParentWnd->GetClientRect(rect);

	int width = rect.Width();
	int height = 0;
	
	height += ConstructHeader(width);
	height += ConstructFooter(width);
	ConstructBody();

	return true;
}



CRect CtlPanel::GetBodyRect()
{
	const CSize padding = globalUtils.ScaleByDPI(CSize(0, 1));

	CRect rect;
	GetClientRect(rect);

	rect.left += padding.cx;
	rect.top += HeaderHeight + padding.cy;
	rect.right -= padding.cx;
	rect.bottom -= FooterHeight;

	return rect;
}



CSize CtlPanel::GetBodySize()
{
	CRect rect;
	GetClientRect(rect);

	return { rect.Width(), rect.Height() - HeaderHeight - FooterHeight };
}



LRESULT CtlPanel::OnDPIChangedAfterParent(WPARAM, LPARAM)
{
	LRESULT result = Default();

	//:TODO

	return result;
}



BOOL CtlPanel::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)Ctl::EColor::MidiumBack);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void CtlPanel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	CSize newSize(cx, cy);
	if (cx > 0 && cy > 0 && OldSize != newSize) {
		OldSize = newSize;
		AdjustLayout(cx, cy);
	}
}
