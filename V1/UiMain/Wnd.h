#pragma once

//--------------------------------------------------------------------------------------------------

namespace Wnd
{
	enum class EAppMode
	{
		All = -1,
		Lite,
		Standard,
		Pro,
	};

	enum class EDocType
	{
		Unknown = -1,
		Model,
		Drawing,
		Image,
		PDF,
	};

	enum class EViewType
	{
		Unknown = -1,
		View3d,
		View2d,
	};

	enum class EStatusArea
	{
		Message = 0,
		Progress,
		Coordinate,
	};

	enum class EMessage
	{
		Unknown = -1,
		OnSignal2d = WM_USER,
		OnSignal3d,
		OnNextFileOpen,
	};



	bool IsAllowedFile(const wchar_t* pFilePath);

	bool IsAllowed3d(const wchar_t* pFilePath);

	bool IsAllowed2d(const wchar_t* pFilePath);
}



struct WindowParams
{
	CString Name;
	RECT Rect;
	CWnd* ParentWnd = nullptr;
	UINT Id = 0;
	// WS_POPUP
	bool Popup = false;
	// WS_EX_TRANSPARENT - The background is not transparent, it sends the signal to the window behind it.
	bool Transparent = false;
	// WS_EX_LAYERED - clear(0) ~ opaque(255)
	BYTE Alpha = 255;
	bool TopMost = false;
	bool Visible = false;

	// Binding Functions
	using OnCharFunc		= void(*)(UINT nChar, UINT nRepCnt, UINT nFlags);
	using OnContextMenuFunc	= void(*)(CWnd*, CPoint point);
	using OnCreateFunc		= int(*)(LPCREATESTRUCT lpCreateStruct);
	using OnEraseBkgndFunc	= BOOL(*)(CDC* pDC);
	using OnKeyDownFunc		= void(*)(UINT nChar, UINT nRepCnt, UINT nFlags);
	using OnKeyUpFunc		= void(*)(UINT nChar, UINT nRepCnt, UINT nFlags);
	using OnLButtonDownFunc	= void(*)(UINT nFlags, CPoint point);
	using OnLButtonUpFunc	= void(*)(UINT nFlags, CPoint point);
	using OnMButtonDownFunc	= void(*)(UINT nFlags, CPoint point);
	using OnMButtonUpFunc	= void(*)(UINT nFlags, CPoint point);
	using OnMouseMoveFunc	= void(*)(UINT nFlags, CPoint point);
	using OnMouseWheelFunc	= BOOL(*)(UINT nFlags, short zDelta, CPoint point);
	using OnPaintFunc		= void(*)();
	using OnRButtonDownFunc	= void(*)(UINT nFlags, CPoint point);
	using OnRButtonUpFunc	= void(*)(UINT nFlags, CPoint point);
	using OnSizeFunc		= void(*)(UINT nType, int cx, int cy);
	using OnSizingFunc		= void(*)(UINT nSide, LPRECT lpRect);

	OnCharFunc			OnChar = nullptr;
	OnContextMenuFunc	OnContextMenu = nullptr;
	OnCreateFunc		OnCreate = nullptr;
	OnEraseBkgndFunc	OnEraseBkgnd = nullptr;
	OnKeyDownFunc		OnKeyDown = nullptr;
	OnKeyUpFunc			OnKeyUp = nullptr;
	OnLButtonDownFunc	OnLButtonDown = nullptr;
	OnLButtonUpFunc		OnLButtonUp = nullptr;
	OnMButtonDownFunc	OnMButtonDown = nullptr;
	OnMButtonUpFunc		OnMButtonUp = nullptr;
	OnMouseMoveFunc		OnMouseMove = nullptr;
	OnMouseWheelFunc	OnMouseWheel = nullptr;
	OnPaintFunc			OnPaint = nullptr;
	OnRButtonDownFunc	OnRButtonDown = nullptr;
	OnRButtonUpFunc		OnRButtonUp = nullptr;
	OnSizeFunc			OnSize = nullptr;
	OnSizingFunc		OnSizing = nullptr;
};
