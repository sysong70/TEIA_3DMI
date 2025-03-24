#include "stdafx.h"
#include "Dio.Manager.h"
#include "Window.Application.h"
#include "Window.View.h"
#include "Window.View2d.h"

//**************************************************************************************************

void DioContainer::Initialize(CWnd* pParent)
{
}



DioItem* DioContainer::Get(Dio::EControlId id)
{
	using namespace Dio;

	switch (id) {
	case EControlId::Length: return &LengthCtl;
	case EControlId::Angle: return &RadiusCtl;

	default:
		RETURN_NULL;
	}
}



CWnd* DioContainer::GetWindow(Dio::EControlId id)
{
	return dynamic_cast<CWnd*>(Get(id));
}



CString DioContainer::GetValue(Dio::EControlId id)
{
	DioItem* pItem = Get(id);
	CString value;

	if (pItem != nullptr) {
		value = pItem->GetValue();
	}

	ASSERT(value.IsEmpty() == false);
	return value;
}



void DioContainer::SetValue(Dio::EControlId id, const CString& value)
{
	DioItem* pItem = Get(id);
	if (pItem != nullptr) {
		pItem->SetValue(value);
		ModifiedItems.push_back(pItem);
	}
}



void DioContainer::Show(Dio::EControlId id, bool show)
{
	::ShowWindow(GetWindow(id)->GetSafeHwnd(), (show ? SW_SHOW : SW_HIDE));
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(WndDynamicInput, CWnd)
	//ON_WM_CHAR()
	ON_WM_ERASEBKGND()
	//ON_WM_KEYDOWN()
	//ON_WM_KEYUP()
	//ON_WM_LBUTTONDOWN()
	//ON_WM_LBUTTONUP()
	//ON_WM_MBUTTONDOWN()
	//ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	//ON_WM_MOUSEWHEEL()
	//ON_WM_PAINT()
	//ON_WM_RBUTTONDOWN()
	//ON_WM_RBUTTONUP()
	//ON_WM_SIZE()
	//ON_WM_TIMER()
END_MESSAGE_MAP()



WndDynamicInput::~WndDynamicInput()
{
}



bool WndDynamicInput::Initialize(CWnd* pParent)
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	{
		wc.style = CS_VREDRAW | CS_HREDRAW;
		wc.lpfnWndProc = AfxWndProc;
		wc.hInstance = AfxGetInstanceHandle();
		wc.hIcon = NULL;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.lpszClassName = L"DynamicInputClassWnd";
	}
	auto result = RegisterClass(&wc);

	const DWORD dwExStyle = WS_EX_TRANSPARENT | WS_EX_PALETTEWINDOW;
	//const DWORD dwExStyle = WS_EX_TRANSPARENT;
	const DWORD dwStyle = WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	//const DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	CString className = wc.lpszClassName;
	CString windowName;
	CRect rect = { 0, 0, 0, 0 };

	if (CreateEx(dwExStyle, className, windowName, dwStyle, rect, NULL, WM_USER) == FALSE) {
		RETURN_FALSE;
	}

	Container.Initialize(this);

	return true;
}



void WndDynamicInput::Show(bool show)
{
	ShowWindow(show ? SW_SHOW : SW_HIDE);
}



void WndDynamicInput::ViewChanged(Window::View* pView)
{
	ViewPtr = pView;

	if (ViewPtr == nullptr) {
		ShowWindow(SW_HIDE);
		return;
	}

	CRect rect;
	ViewPtr->GetClientRect(rect);

	//SetParent(pView);
	SetWindowPos(&wndTopMost, rect.left, rect.top, rect.Width(), rect.Height(), 0);
	ShowWindow(SW_SHOW);
}

//--------------------------------------------------------------------------------------------------

void WndDynamicInput::SetInputs(Json::Object* pData)
{
	using namespace Dio;

	Container.Clear();

	Json::Object& data = *pData;
	Json::Object& options = data.GetAt(SKW_OPTIONS);

	CSize size(70, 24);
	const int lineCount = 5;
	const int arcCount = 4;

	if (auto* pValue = options.FindValue(SKW_LENGTH)) {
		ASSERT(pValue->IsArray());
		Json::Array& values = pValue->AsArray();

		double length = values.GetReal(0);
		ASSERT(values.GetInteger(1) == lineCount);

		int index = 2;
		CPoint points[lineCount];
		for (int i = 0; i < lineCount; i++) {
			points[i].x = values.GetInteger(index++) - size.cx / 2;
			points[i].y = values.GetInteger(index++) - size.cy / 2;
		}

		CWnd* pWnd = Container.GetWindow(EControlId::Length);
		pWnd->SetWindowText(WStr::ToString(length, 3));
		pWnd->MoveWindow(CRect(points[2], size));
		pWnd->ShowWindow(SW_SHOW);
	}
	else {
		Container.Show(EControlId::Length, false);
	}

	if (auto* pValue = options.FindValue(SKW_ANGLE)) {
		ASSERT(pValue->IsArray());
		Json::Array& values = pValue->AsArray();

		double angle = values.GetReal(0);
		ASSERT(values.GetInteger(1) == arcCount);

		int index = 2;
		CPoint points[arcCount];
		for (int i = 0; i < arcCount; i++) {
			points[i].x = values.GetInteger(index++) - size.cx / 2;
			points[i].y = values.GetInteger(index++) - size.cy / 2;
		}

		CWnd* pWnd = Container.GetWindow(EControlId::Angle);
		pWnd->SetWindowText(WStr::ToString(angle, 3));
		pWnd->MoveWindow(CRect(points[2], size));
		pWnd->ShowWindow(SW_SHOW);
	}
	else {
		Container.Show(EControlId::Angle, false);
	}

	AdjustLayout();
}

//--------------------------------------------------------------------------------------------------

BOOL WndDynamicInput::PreCreateWindow(CREATESTRUCT& cs)
{
	if (__super::PreCreateWindow(cs) == FALSE) {
		RETURN_FALSE;
	}

	//:WARNING
	cs.hMenu = NULL;

	return TRUE;
}



BOOL WndDynamicInput::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}



void WndDynamicInput::OnPaint()
{
	CPaintDC dc(this);

	CWnd* pChildWnd = GetWindow(GW_CHILD);
	if (pChildWnd != nullptr) {
		CRect rect;
		pChildWnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		InvalidateRect(&rect, TRUE);
	}

	CRect rect;
	GetClientRect(&rect);

	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* pOldPen = (CPen*)dc.SelectObject(pen);
	CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);

	dc.Rectangle(&rect);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
}



void WndDynamicInput::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	GetView()->OnKeyDown(nChar, nRepCnt, nFlags);
}



void WndDynamicInput::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	GetView()->OnKeyUp(nChar, nRepCnt, nFlags);
}



void WndDynamicInput::OnLButtonDown(UINT nFlags, CPoint point)
{
	LastPoint = point;
	GetView()->OnLButtonDown(nFlags, point);
}



void WndDynamicInput::OnLButtonUp(UINT nFlags, CPoint point)
{
	GetView()->OnLButtonUp(nFlags, point);
}



void WndDynamicInput::OnMButtonDown(UINT nFlags, CPoint point)
{
	LastPoint = point;
	GetView()->OnMButtonDown(nFlags, point);
}



void WndDynamicInput::OnMButtonUp(UINT nFlags, CPoint point)
{
	GetView()->OnMButtonUp(nFlags, point);
}



void WndDynamicInput::OnMouseMove(UINT nFlags, CPoint point)
{
	CurrentPoint = point;
	//GetView()->OnMouseMove(nFlags, point);
}



BOOL WndDynamicInput::OnMouseWheel(UINT nFlags, short zDelta, CPoint point)
{
	return GetView()->OnMouseWheel(nFlags, zDelta, point);
}



void WndDynamicInput::OnRButtonDown(UINT nFlags, CPoint point)
{
	LastPoint = point;
	GetView()->OnRButtonDown(nFlags, point);
}



void WndDynamicInput::OnRButtonUp(UINT nFlags, CPoint point)
{
	GetView()->OnRButtonUp(nFlags, point);
}

//--------------------------------------------------------------------------------------------------

Window::View* WndDynamicInput::GetView()
{
	DEBUG_VALID(ViewPtr);
	return ViewPtr;
}
