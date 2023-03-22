#pragma once

#include <BCGCBProInc.h>



namespace Control
{

	class TabWnd;
	class ToolBar;



	enum class EAlign
	{
		Unknown = 0,
		// move to horizontal direction - x
		HorizontalLeft,
		HorizontalCenter,
		HorizontalRight,
		// move to vertical direction - y
		VerticalTop,
		VerticalCenter,
		VerticalBottom
	};

	enum class EDirection
	{
		Unknown = 0,
		ToRight,
		ToLeft,
		ToBottom,
		ToTop,
	};

	enum class EPivot
	{
		Unknown = 0,
		TopLeft,
		TopCenter,
		TopRight,
		MiddleLeft,
		MiddleCenter,
		MiddleRight,
		BottomLeft,
		BottomCenter,
		BottomRight,
	};

	enum class EColor : COLORREF
	{
		Transparent = 0xFFFFFFFF,
		LightBack = RGB(0x60, 0x60, 0x60),
		MidiumBack = RGB(0x48, 0x48, 0x48),
		DarkBack = RGB(0x30, 0x30, 0x30),
		DialogBack = RGB(0x25, 0x25, 0x25),

		Arsenic = RGB(0x43, 0x43, 0x43),
		BlueBolt = RGB(0x00, 0xAA, 0xFF),
		Charcoal = RGB(0x3B, 0x44, 0x53),
		HonoluluBlue = RGB(0x00, 0x67, 0xAD),
		OldSilver = RGB(0x86, 0x86, 0x86),

		White = RGB(0xFF, 0xFF, 0xFF),
		Red = RGB(0xFF, 0x00, 0x00),
		Green = RGB(0x00, 0xFF, 0x00),
		Blue = RGB(0x00, 0x00, 0xFF),
		Yellow = RGB(0xFF, 0xFF, 0x00),
		Gray = RGB(0x80, 0x80, 0x80),
		LightGray = RGB(0xB0, 0xB0, 0xB0),
		Black = RGB(0, 0, 0),
	};



	// resize and move control
	CRect AdjustLayout(CWnd* pControl, CRect frame, CSize baseSize, EPivot ePivot);
	// move control
	CRect AdjustPosition(CWnd* pControl, CRect frame, EPivot ePivot);
	// resize control
	CSize AdjustSize(CWnd* pControl, CSize baseSize);

	CSize GetControlSize(CWnd* pControl);

	void SetControlSize(CWnd* pControl, CSize size);
	// CBCGPTabWnd tab height
	int TabHeight();
	// CBCGPTreeCtrlEx row height
	int TreeRowHeight();
	// CBCGPPropList row padding
	int PropListRowPadding();
}
