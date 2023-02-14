#include "stdafx.h"
#include "Component.Panel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetPanel

namespace PresetPanel
{
	CSize BodyPadding()
	{
		return globalUtils.ScaleByDPI(CSize(0, 1));
	}
}



using namespace Component;

BEGIN_MESSAGE_MAP(Panel, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
END_MESSAGE_MAP()



Component::Panel::Panel()
{
}



Component::Panel::~Panel()
{
	DestroyWindow();
}



bool Component::Panel::Initialize(CWnd* pParentWnd, UINT id)
{
	if (__super::Create(NULL, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, {}, pParentWnd, id) == FALSE) {
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



void Component::Panel::AdjustLayout(int cx, int cy)
{
	m_toolBar.AdjustLayout();
}



void Component::Panel::ConstructBody()
{
}



int Component::Panel::ConstructFooter(int cy)
{
	return m_nFooterHeight = 0;
}



int Component::Panel::ConstructHeader(int cx)
{
	return m_nHeaderHeight = 0;
}



CRect Component::Panel::GetBodyRect()
{
	CSize padding = PRESET::BodyPadding();
	CRect rect;
	GetClientRect(rect);

	rect.left += padding.cx;
	rect.top += m_nHeaderHeight + padding.cy;
	rect.right -= padding.cx;
	rect.bottom -= m_nFooterHeight;

	return rect;
}



CSize Component::Panel::GetBodySize()
{
	CRect rect;
	GetClientRect(rect);

	return { rect.Width(), rect.Height() - m_nHeaderHeight - m_nFooterHeight };
}



void Component::Panel::PostNcDestroy()
{
	__super::PostNcDestroy();
}



LRESULT Component::Panel::OnDPIChangedAfterParent(WPARAM, LPARAM)
{
	LRESULT result = Default();

	//:TODO

	return result;
}



BOOL Component::Panel::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)EColor::MidiumBack);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void Component::Panel::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	CSize newSize(cx, cy);
	if (cx > 0 && cy > 0 && m_oldSize != newSize) {
		m_oldSize = newSize;
		AdjustLayout(cx, cy);
	}
}

#undef PRESET
