#include "stdafx.h"
#include "Component.Input.h"
#include "Window.View.h"
#include <WStr.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetInputBar

namespace PresetInputBar
{
	const wchar_t* RealFormat = L"%.4f";
}



Component::Input::Input()
{
}



Component::Input::~Input()
{
}



bool Component::Input::SetView(Window::View* pView)
{
	m_pView = pView;

	return true;
}



void Component::Input::SetMode(Signal::EInputMode eMode)
{
	m_eMode = eMode;
	Initialize();
}



void Component::Input::SetValue(double value)
{
	Initialize();
	m_text.Format(PRESET::RealFormat, value);
}



void Component::Input::SetValue(int value)
{
	Initialize();
	m_text.Format(L"%d", value);
}



void Component::Input::SetValue(double x, double y)
{
	Initialize();
	m_texts[1].Format(PRESET::RealFormat, x);
	m_texts[2].Format(PRESET::RealFormat, y);
}



void Component::Input::SetValue(double x, double y, double z)
{
	Initialize();
	m_texts[1].Format(PRESET::RealFormat, x);
	m_texts[2].Format(PRESET::RealFormat, y);
	m_texts[3].Format(PRESET::RealFormat, z);
}



void Component::Input::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsValid() == false) {
		return;
	}
	switch (m_eMode) {
	case Signal::EInputMode::Real:		OnCharReal(nChar, nRepCnt, nFlags);		break;
	case Signal::EInputMode::Integer:	OnCharInteger(nChar, nRepCnt, nFlags);	break;
	case Signal::EInputMode::Point2d:	OnCharPoint2d(nChar, nRepCnt, nFlags);	break;
	case Signal::EInputMode::Point3d:	OnCharPoint3d(nChar, nRepCnt, nFlags);	break;
	default:
		DEBUG_STOP;
	}
}



void Component::Input::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsValid() == false) {
		return;
	}

	CString text = m_text;
	int index = m_textIndex;
	int pos = m_cursorPos;
	int len = m_text.GetLength();

	if (nChar == VK_LEFT) {
		m_cursorPos = max(m_cursorPos - 1, 0);
	}
	else if (nChar == VK_RIGHT) {
		m_cursorPos = min(m_cursorPos + 1, len);
	}
	else if (nChar == VK_HOME) {
		m_cursorPos = 0;
	}
	else if (nChar == VK_END) {
		m_cursorPos = len;
	}
	else if (nChar == VK_DELETE) {
		m_text.Delete(m_cursorPos);
	}
	else if (nChar == VK_BACK) {
		if (m_cursorPos > 0) {
			m_cursorPos--;
			m_cursorPos = max(m_cursorPos, 0);
			m_text.Delete(m_cursorPos);
		}
	}
	else if (nChar == VK_TAB) {
		if (::GetKeyState(VK_SHIFT) & 0x8000) {
			m_textIndex = max(m_textIndex - 1, 0);
		}
		else {
			m_textIndex = min(m_textIndex + 1, 3);
		}
		m_cursorPos = 0;
	}

	SendDataToRenderer(text != m_text || index != m_textIndex || pos != m_cursorPos);
}



void Component::Input::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (IsValid() == false) {
		return;
	}

	SendDataToRenderer();
}



void Component::Input::Initialize()
{
	m_changed = false;

	for (int i = 0; i < 4; i++) {
		m_texts[i].Empty();
	}

	m_text = m_texts[0];
	m_textIndex = 0;
	m_selectIndex = -1;
	m_cursorPos = 0;
}



bool Component::Input::IsValid()
{
	return m_pView != nullptr;
}



bool Component::Input::OnCharReal(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString text = m_text;
	int pos = m_cursorPos;
	int len = m_text.GetLength();

	if (isdigit(nChar)) {
		m_text.Insert(m_cursorPos++, nChar);
	}
	else if (nChar == '-') {
		if (m_cursorPos == 0 && m_text.Find(L"-") == -1) {
			m_text.Insert(m_cursorPos++, nChar);
		}
	}
	else if (nChar == '.') {
		if (m_text.Find(L".") == -1) {
			m_text.Insert(m_cursorPos++, nChar);
		}
	}

	SendDataToRenderer(text != m_text || pos != m_cursorPos);

	return true;
}



bool Component::Input::OnCharInteger(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString text = m_text;
	int pos = m_cursorPos;
	int len = m_text.GetLength();

	if (isdigit(nChar)) {
		m_text.Insert(m_cursorPos++, nChar);
	}
	else if (nChar == '-') {
		if (m_cursorPos == 0 && m_text.Find(L"-") == -1) {
			m_text.Insert(m_cursorPos++, nChar);
		}
	}

	return true;
}



bool Component::Input::OnCharPoint2d(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return true;
}



bool Component::Input::OnCharPoint3d(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return true;
}



void Component::Input::SendDataToRenderer(bool forced)
{
	if ((m_changed || forced) == false) {
		return;
	}

	CString value;
	switch (m_eMode) {
	case Signal::EInputMode::Real:
	case Signal::EInputMode::Integer:
		value = m_texts[0];
		break;

	case Signal::EInputMode::Point2d:
	case Signal::EInputMode::Point3d:
		value = m_texts[1] + L"," + m_texts[2] + (m_eMode == Signal::EInputMode::Point3d ? L"," + m_texts[3] : L"");
		if (m_texts[0] == L"@") {
			value = L"@" + value;
		}
		break;

	default:
		DEBUG_RETURN;
	}

	//m_pView->m_delivery.view.OnInput(value, 0, m_cursorPos);
	m_changed = false;
}
