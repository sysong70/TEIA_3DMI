#pragma once

#include <BCGCBProInc.h>



namespace Layout
{
	class Base;				// CWnd
	class ControlWrapper;	// Layout::Base
	class Stack;			// Layout::Base



	using Bases = std::vector<Base*>;



	enum class EHorizontalAlignment
	{
		Stretch = 0,
		Left,
		Center,
		Right,
	};

	enum class EVerticalAlignment
	{
		Stretch = 0,
		Top,
		Center,
		Bottom,
	};

	enum class EBoxModel
	{
		Content,
		Padding,
		Border,
		Margin,
	};

	enum class EColorType
	{
		Border,
		Background,
	};

	enum class EVisibility
	{
		Visible,
		Hidden,
		Collapsed,
	};



	COLORREF GetTransparent();
}
