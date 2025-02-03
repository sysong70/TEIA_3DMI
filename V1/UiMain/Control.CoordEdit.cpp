#include "stdafx.h"
#include "Control.CoordEdit.h"
#include "Window.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define CHR_COMMA	L','
#define CHR_MINUS	L'-'
#define CHR_PERIOD	L'.'

using namespace Control;

BEGIN_MESSAGE_MAP(CoordEdit, CWnd)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



Control::CoordEdit::CoordEdit()
{
}



Control::CoordEdit::~CoordEdit()
{
}



CString Control::CoordEdit::GetValue()
{
	CString value;
	GetWindowText(value);

	return value;
}



void Control::CoordEdit::SetOriginalValue(const CString& value)
{
	m_sOriginalValue = value;
	SetWindowText(value);
}



void Control::CoordEdit::SetValue(const CString& value)
{
	SetWindowText(value);
}



CString Control::CoordEdit::UpdateValue()
{
	CString oldValue = GetValue();
	WStringArray elements;
	WStr::Split(oldValue, CHR_COMMA, elements);

	for (int i = 0; i < elements.size(); i++) {
		CString value = elements[i];
		CString formatValue = WStr::ToString(WStr::ToDouble(value), 4);

		if (value != formatValue) {
			elements[m_nElementIndex] = formatValue;
		}
	}

	CString newValue = WStr::Join(elements, CHR_COMMA);
	if (oldValue != newValue) {
		// update value
		SetValue(newValue);
	}

	return newValue;
}



void Control::CoordEdit::ClearSelection()
{
	SetCaretIndex(GetCaretIndex());
}



wchar_t Control::CoordEdit::GetCaretChar(int shift)
{
	CString value = GetValue();
	CPoint point = GetCaretPos();
	int charPos = CharFromPos(point) + shift;

	if (charPos >= value.GetLength() || charPos < 0) {
		return 0;
	}
	else {
		return GetValue().GetAt(charPos);
	}
}



int Control::CoordEdit::GetCaretIndex()
{
	return CharFromPos(GetCaretPos());
}



CString Control::CoordEdit::GetActiveElement()
{
	return GetElement(m_nElementIndex);
}



CString Control::CoordEdit::GetElement(int index)
{
	std::vector<Range> ranges;
	GetElementRanges(ranges);
	ASSERT(0 <= index && index < (int)ranges.size());

	int start = ranges[index].Start;
	int count = ranges[index].End - start;

	return GetValue().Mid(start, count);
}



int Control::CoordEdit::GetElementIndex()
{
	CString value = GetValue();

	CPoint point = GetCaretPos();
	int charPos = CharFromPos(point);
	if (value.GetAt(charPos) == CHR_COMMA) {
		charPos--;
	}

	std::vector<Range> ranges;
	GetElementRanges(ranges);

	for (int i = 0; i < ranges.size() - 1; i++) {
		if (ranges[i].Start <= charPos && charPos <= ranges[i].End) {
			return i;
		}
	}

	ASSERT(charPos >= 0);
	return (int)ranges.size() - 1;
}

int Control::CoordEdit::GetElementIndex(Range& range)
{
	CString value = GetValue();

	CPoint point = GetCaretPos();
	int charPos = CharFromPos(point);
	if (value.GetAt(charPos) == CHR_COMMA) {
		charPos--;
	}

	std::vector<Range> ranges;
	GetElementRanges(ranges);

	for (int i = 0; i < ranges.size() - 1; i++) {
		if (ranges[i].Start <= charPos && charPos <= ranges[i].End) {
			range = ranges[i];
			return i;
		}
	}

	ASSERT(charPos >= 0);
	range = ranges.back();
	return (int)ranges.size() - 1;
}

int Control::CoordEdit::GetElementIndex(CPoint point)
{
	CString value = GetValue();

	int charPos = CharFromPos(point);
	if (charPos < 0) {
		return 0xFF;
	}
	if (value.GetAt(charPos) == CHR_COMMA) {
		charPos--;
	}

	std::vector<Range> ranges;
	GetElementRanges(ranges);

	for (int i = 0; i < ranges.size() - 1; i++) {
		if (ranges[i].Start <= charPos && charPos <= ranges[i].End) {
			return i;
		}
	}

	ASSERT(charPos >= 0);
	return int(ranges.size() - 1);
}



CoordEdit::Range Control::CoordEdit::GetElementRange(int index)
{
	std::vector<Range> ranges;
	GetElementRanges(ranges);

	return ranges[index];
}



void Control::CoordEdit::GetElementRanges(std::vector<Range>& ranges)
{
	CString value = GetValue();

	int comma = -1;
	int startPos = 0;
	while ((comma = value.Find(CHR_COMMA, startPos)) != -1) {
		ranges.push_back({ startPos, comma - 1 });
		startPos = comma + 1;
	}

	ranges.push_back({ startPos, value.GetLength() - 1});
}



bool Control::CoordEdit::HasSelection()
{
	DWORD pos = GetSel();
	int start = LOWORD(pos);
	int end = HIWORD(pos);

	return start != -1 && end != -1;
}



void Control::CoordEdit::SelectElement(int index)
{
	std::vector<Range> ranges;
	GetElementRanges(ranges);

	if (index == 0xFF) {
		return;
	}
	else if (index >= (int)ranges.size()) {
		index = 0;
	}
	else if (index < 0) {
		index = (int)ranges.size() - 1;
	}

	if (m_nElementIndex != -1 && m_nElementIndex != index) {
		WStringArray elements;
		WStr::Split(GetValue(), CHR_COMMA, elements);

		CString value = elements[m_nElementIndex];
		CString formatValue = WStr::ToString(WStr::ToDouble(value), 4);
		if (value != formatValue) {
			elements[m_nElementIndex] = formatValue;
			// update value
			SetValue(WStr::Join(elements, CHR_COMMA));
			// refresh range
			ranges.clear();
			GetElementRanges(ranges);
		}
	}

	m_nElementIndex = index;

	SetSel(ranges[index].Start, ranges[index].End + 1);
}



void Control::CoordEdit::SetCaretIndex(int index)
{
	SetSel(index, index);
}



void Control::CoordEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (WStr::IsDigit(nChar) || nChar == VK_BACK) {
	}
	else if (nChar == CHR_PERIOD) {
		if (GetActiveElement().Find(CHR_PERIOD) != -1) {
			return;
		}
	}
	else if (nChar == CHR_MINUS) {
		if (HasSelection() == false) {
		}
	}
	else if (nChar == VK_TAB) {
		SelectElement(GetElementIndex() + (IsKeyPressed(VK_SHIFT) ? -1 : 1));
		return;
	}
	else {
		return;
	}

	__super::OnChar(nChar, nRepCnt, nFlags);
}



void Control::CoordEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_LEFT) {
		// ignore Ctrl + Left
		if (IsKeyPressed(VK_CONTROL)) {
			return;
		}

		if (GetCaretChar(-1) == CHR_COMMA) {
			if (HasSelection() && IsKeyPressed(VK_SHIFT) == false) {
				ClearSelection();
			}

			return;
		}
	}
	else if (nChar == VK_RIGHT) {
		// ignore Ctrl + Right
		if (IsKeyPressed(VK_CONTROL)) {
			return;
		}

		if (GetCaretChar() == CHR_COMMA) {
			if (HasSelection() && IsKeyPressed(VK_SHIFT) == false) {
				ClearSelection();
			}

			return;
		}
	}
	else if (nChar == VK_HOME) {
		ClearSelection();

		Range range = GetElementRange(m_nElementIndex);
		SetCaretIndex(range.Start);
		return;
	}
	else if (nChar == VK_END) {
		ClearSelection();

		Range range = GetElementRange(m_nElementIndex);
		SetCaretIndex(range.End + 1);
		return;
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}



void Control::CoordEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
}



void Control::CoordEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);

	SelectElement(GetElementIndex(point));
}

#undef CHR_COMMA
#undef CHR_MINUS
#undef CHR_PERIOD

