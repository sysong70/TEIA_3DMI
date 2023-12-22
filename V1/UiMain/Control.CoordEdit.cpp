#include "stdafx.h"
#include "Control.CoordEdit.h"
#include "BCGPStdAfx.h"
#include "bcgcbpro.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace Control;

IMPLEMENT_DYNAMIC(CoordEdit, CBCGPEdit)

BEGIN_MESSAGE_MAP(CoordEdit, CBCGPEdit)
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONUP()
	ON_CONTROL_REFLECT(EN_SETFOCUS, OnSetFocus)

	ON_MESSAGE(WM_CLEAR, OnClear)
	ON_MESSAGE(WM_CUT, OnCut)
	ON_MESSAGE(WM_GETTEXT, OnGetText)
	ON_MESSAGE(WM_GETTEXTLENGTH, OnGetTextLength)
	ON_MESSAGE(WM_PASTE, OnPaste)
	ON_MESSAGE(WM_SETTEXT, OnSetText)
END_MESSAGE_MAP()



Control::CoordEdit::CoordEdit()
{
}



Control::CoordEdit::~CoordEdit()
{
}



void Control::CoordEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnChar(nChar, nRepCnt, nFlags);
}



int Control::CoordEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}



void Control::CoordEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}



void Control::CoordEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);
}



void Control::CoordEdit::OnSetFocus()
{
}



LRESULT Control::CoordEdit::OnClear(WPARAM, LPARAM)
{
	return 0;
}



LRESULT Control::CoordEdit::OnCut(WPARAM, LPARAM)
{
	return 0;
}



LRESULT Control::CoordEdit::OnGetText(WPARAM wParam, LPARAM lParam)
{
	return 0;
}



LRESULT Control::CoordEdit::OnGetTextLength(WPARAM, LPARAM)
{
	return 0;
}



LRESULT Control::CoordEdit::OnPaste(WPARAM, LPARAM)
{
	return 0L;
}



LRESULT Control::CoordEdit::OnSetText(WPARAM, LPARAM lParam)
{
	return FALSE;
}
