#include "stdafx.h"
#include "Control.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CRect Control::AdjustLayout(CWnd* pControl, CRect frame, CSize baseSize, EPivot ePivot)
{
	CSize size = GetControlSize(pControl);
	size.cx = max(size.cx, baseSize.cx);
	size.cy = max(size.cy, baseSize.cy);

	CPoint pivot;

	switch (ePivot) {
	case EPivot::TopLeft:
		pivot.x = frame.left;
		pivot.y = frame.top;
		break;

	case EPivot::TopCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.top;
		break;

	case EPivot::TopRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.top;
		break;

	case EPivot::MiddleLeft:
		pivot.x = frame.left;
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case EPivot::MiddleCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case EPivot::MiddleRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case EPivot::BottomLeft:
		pivot.x = frame.left;
		pivot.y = frame.bottom - size.cy;
		break;

	case EPivot::BottomCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.bottom - size.cy;
		break;

	case EPivot::BottomRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.bottom - size.cy;
		break;

	default:
		DEBUG_STOP;
		break;
	}

	// move and resize
	pControl->SetWindowPos(NULL, pivot.x, pivot.y, size.cx, size.cy, 0);

	return { pivot, size };
}



CRect Control::AdjustPosition(CWnd* pControl, CRect frame, EPivot ePivot)
{
	CSize size = GetControlSize(pControl);
	CPoint pivot;

	switch (ePivot) {
	case EPivot::TopLeft:
		pivot.x = frame.left;
		pivot.y = frame.top;
		break;

	case EPivot::TopCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.top;
		break;

	case EPivot::TopRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.top;
		break;

	case EPivot::MiddleLeft:
		pivot.x = frame.left;
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case EPivot::MiddleCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case EPivot::MiddleRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.CenterPoint().y - (size.cy / 2);
		break;

	case EPivot::BottomLeft:
		pivot.x = frame.left;
		pivot.y = frame.bottom - size.cy;
		break;

	case EPivot::BottomCenter:
		pivot.x = frame.CenterPoint().x - (size.cx / 2);
		pivot.y = frame.bottom - size.cy;
		break;

	case EPivot::BottomRight:
		pivot.x = frame.right - size.cx;
		pivot.y = frame.bottom - size.cy;
		break;

	default:
		DEBUG_STOP;
		break;
	}

	pControl->SetWindowPos(NULL, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);

	return { pivot, size };
}



CSize Control::AdjustSize(CWnd* pControl, CSize baseSize)
{
	CSize size = GetControlSize(pControl);
	size.cx = max(size.cx, baseSize.cx);
	size.cy = max(size.cy, baseSize.cy);

	pControl->SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);

	return size;
}



CSize Control::GetControlSize(CWnd* pControl)
{
	DEBUG_VALID(pControl);

	CRect rect;
	if (pControl != nullptr && pControl->GetSafeHwnd() != nullptr) {
		pControl->GetClientRect(&rect);
	}
	else {
		DEBUG_STOP;
	}

	return { rect.Width(), rect.Height() };
}



void Control::SetControlSize(CWnd* pControl, CSize size)
{
	pControl->SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);
}



int Control::TabHeight()
{
	return globalUtils.ScaleByDPI(24);
}



int Control::TreeRowHeight()
{
	return globalUtils.ScaleByDPI(24);
}



int Control::PropListRowPadding()
{
	return globalUtils.ScaleByDPI(1);
}
