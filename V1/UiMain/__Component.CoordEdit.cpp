#include "stdafx.h"
#include "Component.CoordEdit.h"
#include "Window.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

using namespace Component;

#define PRESET PresetCoordEdit

namespace PresetCoordEdit
{
	const UINT Id = WM_USER + 1;
	std::vector<FloatValueEdit*> Controls;



	CSize ControlSize()
	{
		return globalUtils.ScaleByDPI(CSize(72, 20));
	}



	CPoint Offset()
	{
		return globalUtils.ScaleByDPI(CPoint(16, 16));
	}
}

//**************************************************************************************************

IMPLEMENT_DYNAMIC(FloatValueEdit, CBCGPEdit)

BEGIN_MESSAGE_MAP(FloatValueEdit, CBCGPEdit)
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



FloatValueEdit::FloatValueEdit()
{
	m_bVisualManagerStyle = TRUE;
	m_bOnGlass = TRUE;
}



FloatValueEdit::~FloatValueEdit()
{
}



void FloatValueEdit::SetDigit(int value)
{
	m_valueFormat.Format(L"%.%dlf", value);
}



void FloatValueEdit::SetIndex(int value)
{
	m_nIndex = value;
}



CString FloatValueEdit::GetValue()
{
	return m_value;
}



void FloatValueEdit::SetValue(CString value)
{
	double dValue = (value.IsEmpty() ? 0.0 : ::_wtof((LPCTSTR)value));
	m_value.Format(m_valueFormat, dValue);

	SetWindowText(m_value);
}



void Component::FloatValueEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CoordEdit* pOwner = (CoordEdit*)GetOwner();
	DEBUG_VALID(pOwner);

	if (nChar == VK_LEFT && (::GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
		pOwner->ChangeActive(m_nIndex - 1);
	}
	else if (nChar == VK_RIGHT && (::GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
		pOwner->ChangeActive(m_nIndex + 1);
	}
	else if (nChar == VK_TAB) {
		pOwner->ChangeActive(m_nIndex + 1);
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}



void Component::FloatValueEdit::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	CBCGPEditColors colors;
	colors.m_clrBackground = (COLORREF)Control::EColor::MidiumBack;
	SetColorTheme(colors);
}



void Component::FloatValueEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	DEBUG_TRACE(L"FloatValueEdit::OnLButtonDown");
}



void FloatValueEdit::OnSetFocus(CWnd* pOldWnd)
{
//	__super::OnSetFocus(pOldWnd);

	CBCGPEditColors colors;
	colors.m_clrBackground = (COLORREF)Control::EColor::DarkBack;
	SetColorTheme(colors);

	SetSel(0, -1);
	ShowCursor(TRUE);
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CoordEdit, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	//ON_EN_SETFOCUS(WM_USER + 1, OnEnSetFocus)
	//ON_EN_SETFOCUS(WM_USER + 2, OnEnSetFocus)
	//ON_EN_SETFOCUS(WM_USER + 3, OnEnSetFocus)
END_MESSAGE_MAP()



Component::CoordEdit::CoordEdit()
{
	PRESET::Controls.push_back(&m_wndEdit1);
	PRESET::Controls.push_back(&m_wndEdit2);
	PRESET::Controls.push_back(&m_wndEdit3);
}



Component::CoordEdit::~CoordEdit()
{
	PRESET::Controls.clear();
}



void Component::CoordEdit::ChangeActive(int index)
{
	//:WANING - size() is unsigned!
	if (index >= (int)PRESET::Controls.size()) {
		index = 0; // move to first
	}
	else if (index < 0) {
		index = PRESET::Controls.size() - 1; // move to last
	}

	PRESET::Controls[index]->SetFocus();
	m_nActivated = index;
}



void Component::CoordEdit::Clear()
{
	for (auto input : PRESET::Controls) {
		input->SetValue(L"");
	}
}



CString Component::CoordEdit::GetValue()
{
	CString value;

	for (auto input : PRESET::Controls) {
		value += input->GetValue() + L",";
	}

	value.TrimRight(L',');

	return value;
}



void Component::CoordEdit::SetValue(const CString& source, int activate)
{
	m_value = source;

	WStringArray values;
	WStr::Split(source, L',', values);

	for (int i = 0; i < values.size(); i++) {
		PRESET::Controls[i]->SetValue(values[i]);
	}

	if (activate >= 0) {
		ChangeActive(activate);
	}
}



BOOL Component::CoordEdit::DestroyWindow()
{
	for (auto input : PRESET::Controls) {
		input->DestroyWindow();
	}

	return __super::DestroyWindow();
}



BOOL Component::CoordEdit::PreCreateWindow(CREATESTRUCT& cs)
{
	return __super::PreCreateWindow(cs);
}



int Component::CoordEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}

	CWnd* pView = GetParent();
	DEBUG_VALID(pView);

	UINT index = 0;
	CBCGPEditColors colors;
	colors.m_clrBackground = (COLORREF)Control::EColor::MidiumBack;
	DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE;

	for (auto input : PRESET::Controls) {
		if (input->Create(dwStyle, {}, this, PRESET::Id + index) == FALSE) {
			RETURN_FALSE;
		}

		input->SetColorTheme(colors, FALSE);
		input->SetFont(&globalUtils.GetFontRegular(pView));
		input->EnableWindow(TRUE);

		input->SetIndex(index++);
	}

	return 0;
}



void Component::CoordEdit::OnEnSetFocus()
{
	FloatValueEdit* pTarget = DYNAMIC_DOWNCAST(FloatValueEdit, CWnd::GetFocus());
	if (pTarget != nullptr) {
		TRACE(pTarget->GetValue());
	}
}



void Component::CoordEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	DEBUG_TRACE(L"CoordEdit::OnKeyDown");
}



void Component::CoordEdit::OnSetFocus(CWnd* pOldWnd)
{
	PRESET::Controls[0]->SetFocus();
}



void Component::CoordEdit::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	int x = 0;
	int width = cx / (m_format == EFormat::Point3d ? 3 : 2);

	for (auto input : PRESET::Controls) {
		input->SetWindowPos(NULL, x, 0, width, cy, SWP_NOZORDER);
		x += width;
	}
}

#undef PRESET
