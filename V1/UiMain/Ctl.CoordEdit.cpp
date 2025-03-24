#include "stdafx.h"

#include "Ctl.CoordEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define CHR_COMMA	L','
#define CHR_MINUS	L'-'
#define CHR_PERIOD	L'.'

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlCoordEdit, CWnd)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()



CString CtlCoordEdit::GetValue()
{
	CString value;
	GetWindowText(value);

	return value;
}



void CtlCoordEdit::SetOriginalValue(const CString& value)
{
	OriginalValue = value;
	SetWindowText(value);
}



void CtlCoordEdit::SetValue(const CString& value)
{
	SetWindowText(value);
}



CString CtlCoordEdit::UpdateValue()
{
	CString oldValue = GetValue();
	WStringArray elements;
	WStr::Split(oldValue, CHR_COMMA, elements);

	for (int i = 0; i < elements.size(); i++) {
		CString value = elements[i];
		CString formatValue = WStr::ToString(WStr::ToDouble(value), 4);

		if (value != formatValue) {
			elements[ElementIndex] = formatValue;
		}
	}

	CString newValue = WStr::Join(elements, CHR_COMMA);
	if (oldValue != newValue) {
		// update value
		SetValue(newValue);
	}

	return newValue;
}



void CtlCoordEdit::ClearSelection()
{
	SetCaretIndex(GetCaretIndex());
}



wchar_t CtlCoordEdit::GetCaretChar(int shift)
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



int CtlCoordEdit::GetCaretIndex()
{
	return CharFromPos(GetCaretPos());
}



CString CtlCoordEdit::GetActiveElement()
{
	return GetElement(ElementIndex);
}



CString CtlCoordEdit::GetElement(int index)
{
	std::vector<Range> ranges;
	GetElementRanges(ranges);
	ASSERT(0 <= index && index < (int)ranges.size());

	int start = ranges[index].Start;
	int count = ranges[index].End - start;

	return GetValue().Mid(start, count);
}



int CtlCoordEdit::GetElementIndex()
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

int CtlCoordEdit::GetElementIndex(Range& range)
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

int CtlCoordEdit::GetElementIndex(CPoint point)
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



CtlCoordEdit::Range CtlCoordEdit::GetElementRange(int index)
{
	std::vector<Range> ranges;
	GetElementRanges(ranges);

	return ranges[index];
}



void CtlCoordEdit::GetElementRanges(std::vector<Range>& ranges)
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



bool CtlCoordEdit::HasSelection()
{
	DWORD pos = GetSel();
	int start = LOWORD(pos);
	int end = HIWORD(pos);

	return start != -1 && end != -1;
}



void CtlCoordEdit::SelectElement(int index)
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

	if (ElementIndex != -1 && ElementIndex != index) {
		WStringArray elements;
		WStr::Split(GetValue(), CHR_COMMA, elements);

		CString value = elements[ElementIndex];
		CString formatValue = WStr::ToString(WStr::ToDouble(value), 4);
		if (value != formatValue) {
			elements[ElementIndex] = formatValue;
			// update value
			SetValue(WStr::Join(elements, CHR_COMMA));
			// refresh range
			ranges.clear();
			GetElementRanges(ranges);
		}
	}

	ElementIndex = index;

	SetSel(ranges[index].Start, ranges[index].End + 1);
}



void CtlCoordEdit::SetCaretIndex(int index)
{
	SetSel(index, index);
}



void CtlCoordEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
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
		SelectElement(GetElementIndex() + (Ctl::IsKeyPressed(VK_SHIFT) ? -1 : 1));
		return;
	}
	else {
		return;
	}

	__super::OnChar(nChar, nRepCnt, nFlags);
}



void CtlCoordEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_LEFT) {
		// ignore Ctrl + Left
		if (Ctl::IsKeyPressed(VK_CONTROL)) {
			return;
		}

		if (GetCaretChar(-1) == CHR_COMMA) {
			if (HasSelection() && Ctl::IsKeyPressed(VK_SHIFT) == false) {
				ClearSelection();
			}

			return;
		}
	}
	else if (nChar == VK_RIGHT) {
		// ignore Ctrl + Right
		if (Ctl::IsKeyPressed(VK_CONTROL)) {
			return;
		}

		if (GetCaretChar() == CHR_COMMA) {
			if (HasSelection() && Ctl::IsKeyPressed(VK_SHIFT) == false) {
				ClearSelection();
			}

			return;
		}
	}
	else if (nChar == VK_HOME) {
		ClearSelection();

		Range range = GetElementRange(ElementIndex);
		SetCaretIndex(range.Start);
		return;
	}
	else if (nChar == VK_END) {
		ClearSelection();

		Range range = GetElementRange(ElementIndex);
		SetCaretIndex(range.End + 1);
		return;
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}



void CtlCoordEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	__super::OnLButtonDown(nFlags, point);
}



void CtlCoordEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	__super::OnLButtonUp(nFlags, point);

	SelectElement(GetElementIndex(point));
}

#undef CHR_COMMA
#undef CHR_MINUS
#undef CHR_PERIOD
