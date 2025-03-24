#pragma once

#include "Ctl.h"

//--------------------------------------------------------------------------------------------------

class CtlCoordEdit : public CBCGPEdit
{
public:

	enum class EType
	{
		Point2d = 2,
		Point3d = 3,
	};

	struct Range
	{
		int Start = -1;
		int End = -1;
	};

	CString OriginalValue;
	CString Value;
	EType PointType = EType::Point3d;
	// index of active element
	int ElementIndex = -1;

public:

	CtlCoordEdit() {}

	~CtlCoordEdit() override {}

public:

	CString GetValue();

	void SetOriginalValue(const CString& value);

	void SetValue(const CString& value);

	CString UpdateValue();

public: // Coordinate element

	void ClearSelection();

	wchar_t GetCaretChar(int shift = 0);

	int GetCaretIndex();

	CString GetActiveElement();

	CString GetElement(int index);

	int GetElementIndex();

	int GetElementIndex(Range& range);

	int GetElementIndex(CPoint point);

	Range GetElementRange(int index);

	void GetElementRanges(std::vector<Range>& ranges);

	bool HasSelection();

	void SelectElement(int index);

	void SetCaretIndex(int index);

public:

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP();
};
