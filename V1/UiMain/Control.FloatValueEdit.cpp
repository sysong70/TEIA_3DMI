#include "stdafx.h"
#include "Control.FloatValueEdit.h"
#include "Component.CoordEdit.h"
#include "BCGPStdAfx.h"
#include "bcgcbpro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------------

using namespace Control;

IMPLEMENT_DYNAMIC(FloatValueEdit, CBCGPEdit)

BEGIN_MESSAGE_MAP(FloatValueEdit, CBCGPEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_SETFOCUS()
	//ON_WM_LBUTTONUP()
	//ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)

	//ON_MESSAGE(WM_CLEAR, OnClear)
	//ON_MESSAGE(WM_CUT, OnCut)
	//ON_MESSAGE(WM_GETTEXT, OnGetText)
	//ON_MESSAGE(WM_GETTEXTLENGTH, OnGetTextLength)
	//ON_MESSAGE(WM_KILLFOCUS, OnKillFocus)
	//ON_MESSAGE(WM_PASTE, OnPaste)
	//ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()



Control::FloatValueEdit::FloatValueEdit()
{
	m_bVisualManagerStyle = TRUE;
	m_bOnGlass = TRUE;
}



Control::FloatValueEdit::~FloatValueEdit()
{
}



void Control::FloatValueEdit::SetDigit(int value)
{
	m_valueFormat.Format(L"%.%dlf", value);
}



void Control::FloatValueEdit::SetIndex(int value)
{
	m_nIndex = value;
}



CString Control::FloatValueEdit::GetValue()
{
	return m_value;
}



void Control::FloatValueEdit::SetValue(CString value)
{
	double dValue = (value.IsEmpty() ? 0.0 : ::_wtof((LPCTSTR)value));
	m_value.Format(m_valueFormat, dValue);

	SetWindowText(m_value);
}



void Control::FloatValueEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_TAB) {
		((Component::CoordEdit*)GetParent())->ChangeActive(m_nIndex + 1);
	}
	else {
		__super::OnChar(nChar, nRepCnt, nFlags);
	}
}



void Control::FloatValueEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}



//void Control::FloatValueEdit::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	__super::OnLButtonUp(nFlags, point);
//}



void Control::FloatValueEdit::OnSetFocus(CWnd* pOldWnd)
{
	__super::OnSetFocus(pOldWnd);
	__super::SetSel(0, -1);

	PostMessage(WM_NCPAINT);
}



//LRESULT Control::FloatValueEdit::OnClear(WPARAM, LPARAM)
//{
//	return 0;
//}
//
//
//
//LRESULT Control::FloatValueEdit::OnCut(WPARAM, LPARAM)
//{
//	return 0;
//}
//
//
//
//LRESULT Control::FloatValueEdit::OnGetText(WPARAM wParam, LPARAM lParam)
//{
//	return 0;
//}
//
//
//
//LRESULT Control::FloatValueEdit::OnGetTextLength(WPARAM, LPARAM)
//{
//	return 0;
//}
//
//
//
//LRESULT Control::FloatValueEdit::OnKillFocus(WPARAM, LPARAM)
//{
//	return 0;
//}
//
//
//
//LRESULT Control::FloatValueEdit::OnPaste(WPARAM, LPARAM)
//{
//	return 0L;
//}
//
//
//
//LRESULT Control::FloatValueEdit::OnSetText(WPARAM, LPARAM lParam)
//{
//	SetValue((LPCTSTR)lParam);
//
//	if (m_value.Compare((LPCTSTR)lParam) != 0) {
//		return ::SetWindowText(GetSafeHwnd(), m_value);
//	}
//
//	return FALSE;
//}
