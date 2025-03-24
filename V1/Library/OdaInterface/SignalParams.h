#pragma once

#include "Signal2d.h"

#include <atltypes.h>

//--------------------------------------------------------------------------------------------------

class SignalParams
{
public:

	Sgn::ETarget Target = Sgn::ETarget::Unknown;
	int Action = -1; // enum Action
	int ViewId = -1;
	bool Valid = false;

public:

	// WARING - delete pointer after use
	static SignalParams* CreateInstance(Json::Object& content, int& signalTarget, int& signalAction);

	SignalParams() {}

	SignalParams(Json::Object& content);

	~SignalParams();

	virtual CString Dump();

#ifdef _DEBUG
	void DebugTrace()
	{
		TRACE(Dump());
	}
#endif
};

//--------------------------------------------------------------------------------------------------

class CommandSignal : public SignalParams
{
public:

	int Id = -1;
	Json::Object Options;

public:

	CommandSignal(Json::Object& content);

	CString Dump() override;
};

//--------------------------------------------------------------------------------------------------

class ContextCommandSignal : public CommandSignal
{
public:

	ContextCommandSignal(Json::Object& content);
};

//--------------------------------------------------------------------------------------------------

class InitializeSignal : public SignalParams
{
public:

	HWND WindowHandle = nullptr;
	CString FilePath;

public:

	InitializeSignal(Json::Object& content);

	CString Dump() override;
};

//--------------------------------------------------------------------------------------------------

class KeyboardSignal : public SignalParams
{
public:

	UINT Flags = 0;
	UINT Repeat = 0;
	UINT Char = 0;
	bool Down = false;

public:

	KeyboardSignal(Json::Object& content);

	bool IsEnter() { return Char == VK_RETURN; }

	bool IsEscape() { return Char == VK_ESCAPE; }

	CString Dump() override;
};

//--------------------------------------------------------------------------------------------------

class MouseSignal : public SignalParams
{
public:

	UINT Flags = 0;
	int X = INT_MIN;
	int Y = INT_MIN;
	int Delta = 0; // wheel

public:

	MouseSignal(Json::Object& content);

	CPoint GetPoint();

	CString Dump() override;

public:

	bool ControlKey() { return Flags & MK_CONTROL; }

	bool ShiftKey() { return Flags & MK_SHIFT; }

	bool AltKey() { return Flags & MK_ALT; }

	bool LeftButton() { return Flags & MK_LBUTTON; }

	bool MiddleButton() { return Flags & MK_MBUTTON; }

	bool RightButton() { return Flags & MK_RBUTTON; }
};

//--------------------------------------------------------------------------------------------------

class PaintSignal : public SignalParams
{
public:

	int Left = 0;
	int Top = 0;
	int Right = 0;
	int Bottom = 0;

	Json::Object Options;

public:

	PaintSignal(Json::Object& content);
	// Call by Renderer::PostPaintSignal()
	PaintSignal(int viewId);

public:

	CString Dump() override;

public:

	CRect GetRect() { return { Left, Top, Right, Bottom }; }
};

//--------------------------------------------------------------------------------------------------

class ResizeSignal : public SignalParams
{
public:

	int Width = 0;
	int Height = 0;

public:

	ResizeSignal(Json::Object& content);

	CString Dump() override;
};

//--------------------------------------------------------------------------------------------------

class TextSignal : public SignalParams
{
public:

	CString Buffer;

public:

	TextSignal(Json::Object& content);

public:

	CString Dump() override;
};
