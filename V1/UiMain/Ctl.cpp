#include "stdafx.h"

#include "Ast.h"
#include "Ctl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

CSize Ctl::Setup(CBCGPButton& control, const CString& title, UINT id, CWnd* pParent)
{
	const CSize defaultSize = CSize(64, 0);
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;

	if (control.GetSafeHwnd() == nullptr) {
		if (control.Create(title, dwStyle, {}, pParent, id) == FALSE) {
			RETURN({});
		}
	}

	control.m_bVisualManagerStyle = TRUE;
	control.SizeToContent();

	return AdjustSize(&control, globalUtils.ScaleByDPI(defaultSize));
}

CSize Ctl::Setup(CBCGPButton& control, Json::Object& data, CWnd* pParent)
{
	return Setup(control, Ast::GetTitle(data), Ast::GetId(data), pParent);
}



CRect Ctl::Setup(CBCGPEdit& control, UINT id, EPivot ePivot, CRect rect, CWnd* pParent)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;

	if (control.GetSafeHwnd() == nullptr) {
		if (control.Create(dwStyle, {}, pParent, id) == FALSE) {
			RETURN({});
		}
	}

	// CHECK
	control.m_bVisualManagerStyle = TRUE;
	control.SetVerticalAlignment(TA_CENTER);

	return AdjustLayout(&control, rect, CSize(rect.Width(), 0), ePivot);
}

CRect Ctl::Setup(CBCGPEdit& control, Json::Object& data, EPivot ePivot, CRect rect, CWnd* pParent)
{
	return Setup(control, Ast::GetId(data), ePivot, rect, pParent);
}



CRect Ctl::Setup(CBCGPStatic& control, const CString& title, EPivot ePivot, CRect rect, CWnd* pParent)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;

	if (control.GetSafeHwnd() == nullptr) {
		if (control.Create(L"", dwStyle, {}, pParent) == FALSE) {
			RETURN({});
		}
	}

	control.m_bVisualManagerStyle = TRUE;
	control.SetWindowText(title);
	control.SizeToContent();

	return AdjustLayout(&control, rect, CSize(rect.Width(), 0), ePivot);
}

CRect Ctl::Setup(CBCGPStatic& control, Json::Object& data, EPivot ePivot, CRect rect, CWnd* pParent)
{
	return Setup(control, Ast::GetTitle(data), ePivot, rect, pParent);
}

//--------------------------------------------------------------------------------------------------

CRect Ctl::AdjustLayout(CWnd* pControl, CRect frame, CSize baseSize, EPivot ePivot, bool update)
{
	CSize size = GetSize(pControl);
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
	if (update) {
		pControl->SetWindowPos(NULL, pivot.x, pivot.y, size.cx, size.cy, SWP_NONE);
	}

	return { pivot, size };
}



CRect Ctl::AdjustPosition(CWnd* pControl, CRect frame, EPivot ePivot)
{
	CSize size = GetSize(pControl);
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



CSize Ctl::AdjustSize(CWnd* pControl, CSize baseSize)
{
	CSize size = GetSize(pControl);
	size.cx = max(size.cx, baseSize.cx);
	size.cy = max(size.cy, baseSize.cy);

	pControl->SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);

	return size;
}



CRect Ctl::Align(Controls controls, CPoint basePoint, EAlign eAlign, CWnd* pParent)
{
	CRect boundary;

	for (auto pControl : controls) {
		CRect controlFrame = GetRect(pControl);
		CSize controlSize = controlFrame.Size();
		CPoint pivot;

		switch (eAlign) {
		case Ctl::EAlign::HorizontalLeft:
			pivot.x = basePoint.x;
			pivot.y = controlFrame.top;
			break;

		case Ctl::EAlign::HorizontalCenter:
			pivot.x = basePoint.x - (controlSize.cx / 2);
			pivot.y = controlFrame.top;
			break;

		case Ctl::EAlign::HorizontalRight:
			pivot.x = basePoint.x - controlSize.cx;
			pivot.y = controlFrame.top;
			break;

		case Ctl::EAlign::VerticalTop:
			pivot.x = controlFrame.left;
			pivot.y = basePoint.y;
			break;

		case Ctl::EAlign::VerticalCenter:
			pivot.x = controlFrame.left;
			pivot.y = basePoint.y - (controlSize.cy / 2);
			break;

		case Ctl::EAlign::VerticalBottom:
			pivot.x = controlFrame.left;
			pivot.y = basePoint.y - controlSize.cy;
			break;

		default:
			DEBUG_STOP;
			break;
		}

		pControl->SetWindowPos(NULL, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);
		// recalculate boundary 
		controlFrame = GetRect(pControl);

		boundary.left = min(boundary.left, controlFrame.left);
		boundary.top = min(boundary.top, controlFrame.top);
		boundary.right = max(boundary.right, controlFrame.right);
		boundary.bottom = max(boundary.bottom, controlFrame.bottom);
	}

	return boundary;
}



CRect Ctl::Destribute(Controls controls, CPoint basePoint, int gap, EDirection eDir, CWnd* pParent)
{
	CRect boundary;
	CPoint offset = basePoint;

	for (auto pControl : controls) {
		CRect controlFrame = GetRect(pControl);
		CSize size = controlFrame.Size();
		CPoint pivot;

		switch (eDir) {
		case Ctl::EDirection::ToRight:
			pivot.x = offset.x;
			pivot.y = controlFrame.top;
			offset.x += size.cx + gap;
			break;

		case Ctl::EDirection::ToLeft:
			pivot.x = offset.x - size.cx;
			pivot.y = controlFrame.top;
			offset.x -= size.cx + gap;
			break;

		case Ctl::EDirection::ToBottom:
			pivot.x = controlFrame.left;
			pivot.y = offset.y;
			offset.y += size.cy + gap;
			break;

		case Ctl::EDirection::ToTop:
			pivot.x = controlFrame.left;
			pivot.y = offset.y - size.cy;
			offset.y -= size.cy + gap;
			break;

		default:
			DEBUG_STOP;
			break;
		}

		pControl->SetWindowPos(NULL, pivot.x, pivot.y, 0, 0, SWP_NOSIZE);
		// recalculate boundary 
		controlFrame = GetRect(pControl);

		boundary.left = min(boundary.left, controlFrame.left);
		boundary.top = min(boundary.top, controlFrame.top);
		boundary.right = max(boundary.right, controlFrame.right);
		boundary.bottom = max(boundary.bottom, controlFrame.bottom);
	}

	return boundary;
}



CRect Ctl::GetRect(CWnd* pControl)
{
	CRect rect;
	if (pControl != nullptr && pControl->GetSafeHwnd() != nullptr && pControl->GetParent() != nullptr) {
		pControl->GetWindowRect(&rect);
		pControl->GetParent()->ScreenToClient(rect);
	}
	else {
		DEBUG_STOP;
	}

	return rect;
}



CSize Ctl::GetSize(CWnd* pControl)
{
	CRect rect;
	if (pControl != nullptr && pControl->GetSafeHwnd() != nullptr) {
		pControl->GetClientRect(&rect);
	}
	else {
		DEBUG_STOP;
	}

	return { rect.Width(), rect.Height() };
}



void Ctl::SetSize(CWnd* pControl, CSize size)
{
	pControl->SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);
}

//--------------------------------------------------------------------------------------------------

int Ctl::BarHeaderHeight()
{
	return globalUtils.ScaleByDPI(34);
}



int Ctl::BarFooterHeight()
{
	return globalUtils.ScaleByDPI(34);
}



int Ctl::BarPadding()
{
	return globalUtils.ScaleByDPI(6);
}



CSize Ctl::Gap()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}



CSize Ctl::ImageSize()
{
	return globalUtils.ScaleByDPI(CSize(24, 24));
}



int Ctl::TabHeight()
{
	return globalUtils.ScaleByDPI(24);
}

//--------------------------------------------------------------------------------------------------

CPoint Ctl::GetMousePos()
{
	CPoint point;
	GetCursorPos(&point);

	return point;
}



bool Ctl::IsKeyPressed(int vk)
{
	return ::GetAsyncKeyState(vk) & 0x8000;
}



wchar_t Ctl::ToVirtualKeyToChar(UINT virtualKey, UINT flags)
{
	BYTE keyState[256];
	BOOL success = ::GetKeyboardState(keyState);
	ASSERT(success);

	HKL keyboardlayout = GetKeyboardLayout(0);
	DEBUG_VALID(keyboardlayout);
	UINT scanCode = flags & 0xffff;

	wchar_t keyBuffer[256] = { 0 };
	int result = ToUnicodeEx(virtualKey, scanCode, (PBYTE)&keyState, (LPWSTR)&keyBuffer, sizeof(keyBuffer) / 16, 0, keyboardlayout);

	return result > 0 ? keyBuffer[0] : 0;
}

//--------------------------------------------------------------------------------------------------

void Ctl::SetTransparentWindow(CWnd* pWindow, int alpha)
{
	if (pWindow == nullptr || pWindow->GetSafeHwnd() == nullptr) {
		return;
	}

	LONG oldStyle = GetWindowLong(pWindow->GetSafeHwnd(), GWL_EXSTYLE);
	LONG ret = SetWindowLong(pWindow->GetSafeHwnd(), GWL_EXSTYLE, oldStyle | WS_EX_LAYERED);
	DWORD error = GetLastError();

	BOOL success = ::SetLayeredWindowAttributes(pWindow->GetSafeHwnd(), 0, (BYTE)(((100 - alpha) * 25) / 10), LWA_ALPHA);
	ASSERT(success);
}
