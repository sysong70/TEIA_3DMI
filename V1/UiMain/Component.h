#pragma once

#include <BCGCBProInc.h>



namespace Component
{
	class LayerPanel;
	class ModelTreePanel;
	class ScenePanel;
	class ViewPanel;

	class HistoryBar;
	class PanelBar;
	class PropertyBar;
	class RibbonBar;
	class StatusBar;
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

#pragma region Utility

	enum class EColor : COLORREF
	{
		LightBack = RGB(0x60, 0x60, 0x60),
		MidiumBack = RGB(0x48, 0x48, 0x48),
		DarkBack = RGB(0x30, 0x30, 0x30),

		Arsenic = RGB(0x43, 0x43, 0x43),
		Charcoal = RGB(0x3B, 0x44, 0x53),

		White = RGB(0xFF, 0xFF, 0xFF),
		Red = RGB(0xFF, 0x00, 0x00),
		Green = RGB(0x00, 0xFF, 0x00),
		Blue = RGB(0x00, 0x00, 0xFF),
		Black = RGB(0, 0, 0),
	};

#pragma endregion //:REGION
}
