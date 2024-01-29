#include "stdafx.h"
#include "Component.CoordEdit.h"
#include "Window.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetCoordEdit

namespace PresetCoordEdit
{
	const UINT Id = WM_USER + 1;

	CSize ControlSize()
	{
		return globalUtils.ScaleByDPI(CSize(72, 20));
	}

	CPoint Offset()
	{
		return globalUtils.ScaleByDPI(CPoint(16, 16));
	}

	std::vector<Control::FloatValueEdit*> Controls;
}



using namespace Component;

BEGIN_MESSAGE_MAP(CoordEdit, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()

	ON_EN_SETFOCUS(WM_USER + 1, OnEnSetFocus)
	ON_EN_SETFOCUS(WM_USER + 2, OnEnSetFocus)
	ON_EN_SETFOCUS(WM_USER + 3, OnEnSetFocus)
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
	if (index >= PRESET::Controls.size()) {
		index = 0;
	}

	CBCGPEditColors colors;
	colors.m_clrBackground = (COLORREF)Control::EColor::MidiumBack;
	PRESET::Controls[m_nActivated]->SetColorTheme(colors);

	m_nActivated = index;
	colors.m_clrBackground = (COLORREF)Control::EColor::DarkBack;
	PRESET::Controls[m_nActivated]->SetColorTheme(colors);
	PRESET::Controls[m_nActivated]->SendMessage(WM_SETFOCUS);
	PRESET::Controls[m_nActivated]->SetSel(0, -1);
}



void Component::CoordEdit::Clear()
{
	for (auto input : PRESET::Controls) {
		//input->SetWindowText(L"");
		input->SetValue(L"");
	}
}



CString Component::CoordEdit::GetValue()
{
	CString comp;
	CString value;

	for (auto input : PRESET::Controls) {
		input->GetValue();
		value += comp + L",";
	}

	value.TrimRight(L',');

	return value;
}



void Component::CoordEdit::SetValue(const CString& source)
{
	m_value = source;

	Coordinates value;
	int tokenPos = 0;

	while (true) {
		CString comp = source.Tokenize(L",", tokenPos);
		if (tokenPos > -1) {
			value.push_back(comp);
		}
		else {
			break;
		}
	}

	for (int i = 0; i < value.size(); i++) {
		//PRESET::Controls[i]->SetWindowText(value[i]);
		PRESET::Controls[i]->SetValue(value[i]);
	}
}



BOOL Component::CoordEdit::DestroyWindow()
{
	for (auto input : PRESET::Controls) {
		input->DestroyWindow();
	}

	return TRUE;
}



BOOL Component::CoordEdit::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= WS_TABSTOP;

	return CWnd::PreCreateWindow(cs);
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

	ChangeActive(0);

	return 0;
}



void Component::CoordEdit::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	int x = 0;
	int width = cx / 3;

	for (auto input : PRESET::Controls) {
		input->SetWindowPos(NULL, x, 0, width, cy, SWP_NOZORDER);
		x += width;
	}
}



void Component::CoordEdit::OnEnSetFocus()
{
	CWnd* pWnd = GetFocus();
	int i = pWnd->GetDlgCtrlID();
	TRACE(L"%d\n", i);
}

#undef PRESET
