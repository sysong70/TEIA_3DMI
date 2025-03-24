#pragma once

#define COLOR(R,G,B) RGB(0x##R, 0x##G, 0x##B)

//--------------------------------------------------------------------------------------------------

namespace Ctl
{
	using Controls = std::list<CWnd*>;
	using ResourceIds = std::vector<UINT>;

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
		Transparent		= 0xFFFFFFFF,
		LightBack		= COLOR(60, 60, 60),
		MidiumBack		= COLOR(48, 48, 48),
		DarkBack		= COLOR(30, 30, 30),
		DialogBack		= COLOR(25, 25, 25),

		Arsenic			= COLOR(43, 43, 43),
		BlueBolt		= COLOR(00, AA, FF),
		Charcoal		= COLOR(3B, 44, 53),
		DeepSkyBlue		= COLOR(00, A8, FF),
		GhostWhite		= COLOR(F8, F8, FF),
		HonoluluBlue	= COLOR(00, 67, AD),
		IndianRed		= COLOR(CD, 5C, 5C),
		NavyBlue		= COLOR(00, 78, D7),
		OldSilver		= COLOR(86, 86, 86),

		White			= COLOR(FF, FF, FF),
		Red				= COLOR(FF, 00, 00),
		Green			= COLOR(00, FF, 00),
		Blue			= COLOR(00, 00, FF),
		Yellow			= COLOR(FF, FF, 00),

		DimGray			= COLOR(69, 69, 69),
		Gray			= COLOR(80, 80, 80),
		DarkGray		= COLOR(A9, A9, A9),
		Silver			= COLOR(C0, C0, C0),
		LightGray		= COLOR(D3, D3, D3),
		Black			= 0x0,
	};

// Create window of control

	CSize Setup(CBCGPButton& control, const CString& title, UINT id, CWnd* pParent = nullptr);

	CSize Setup(CBCGPButton& control, Json::Object& data, CWnd* pParent = nullptr);

	CRect Setup(CBCGPEdit& control, UINT id, EPivot ePivot, CRect rect, CWnd* pParent = nullptr);

	CRect Setup(CBCGPEdit& control, Json::Object& data, EPivot ePivot, CRect rect, CWnd* pParent = nullptr);

	CRect Setup(CBCGPStatic& control, const CString& title, EPivot ePivot, CRect rect, CWnd* pParent = nullptr);

	CRect Setup(CBCGPStatic& control, Json::Object& data, EPivot ePivot, CRect rect, CWnd* pParent = nullptr);

// Position and arrange controls

	// resize and move control
	CRect AdjustLayout(CWnd* pControl, CRect frame, CSize baseSize, EPivot ePivot, bool update = true);
	// move control
	CRect AdjustPosition(CWnd* pControl, CRect frame, EPivot ePivot);
	// resize control
	CSize AdjustSize(CWnd* pControl, CSize baseSize);
	// align controls at base point
	CRect Align(Controls controls, CPoint basePoint, EAlign eAlign, CWnd* pParent);
	// destribute controls from base point with gap
	CRect Destribute(Controls controls, CPoint basePoint, int gap, EDirection eDir, CWnd* pParent);
	// boundary on parent area
	CRect GetRect(CWnd* pControl);

	CSize GetSize(CWnd* pControl);

	void SetSize(CWnd* pControl, CSize size);

// Const values

	// Custom bar title height
	int BarHeaderHeight();

	int BarFooterHeight();

	int BarPadding();

	CSize Gap();
	// Default image size
	CSize ImageSize();
	// CBCGPTabWnd tab height
	int TabHeight();

// Keyboard and Mouse

	CPoint GetMousePos();

	bool IsKeyPressed(int vk);

	wchar_t ToVirtualKeyToChar(UINT virtualKey, UINT flags);

// Window state

	void SetTransparentWindow(CWnd* pWindow, int alpha);
}

#undef COLOR
