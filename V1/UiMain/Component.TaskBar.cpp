#include "stdafx.h"
#include "Component.TaskBar.h"
#include "Window.MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetTaskBar

namespace PresetTaskBar
{
	enum ControlId
	{
		Id = WM_USER,
	};

	int BottomOffset()
	{
		return globalUtils.ScaleByDPI(200);
	}

	CSize Margin()
	{
		return globalUtils.ScaleByDPI(CSize(0, 1));
	}

	CSize MinBarSize()
	{
		return globalUtils.ScaleByDPI(CSize(400, 600));
	}
}



using namespace Component;

BEGIN_MESSAGE_MAP(TaskBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
END_MESSAGE_MAP()



Component::TaskBar::TaskBar()
{
}



Component::TaskBar::~TaskBar()
{
	DestroyWindow();
}



bool Component::TaskBar::Initialize(CWnd* pParentWnd)
{
	bool success = __super::Create(NULL, L"", WS_CHILD | WS_CLIPCHILDREN, {}, pParentWnd, PRESET::Id);
	if (success == false) {
		RETURN_FALSE;
	}

	return true;
}



CSize Component::TaskBar::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr || IsWindowVisible() == false ||
		GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr) {
		return {};
	}

	CRect rect;
	GetParent()->GetClientRect(rect);

	CSize size = PRESET::MinBarSize();
	size.cy = rect.Height() - PRESET::BottomOffset();
	SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);

	return size;
}



void Component::TaskBar::PostNcDestroy()
{
	__super::PostNcDestroy();
}



LRESULT Component::TaskBar::OnDPIChangedAfterParent(WPARAM, LPARAM)
{
	LRESULT result = Default();

	//:TODO

	return result;
}



BOOL Component::TaskBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)EColor::DarkBack);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void Component::TaskBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);
}

#undef PRESET
