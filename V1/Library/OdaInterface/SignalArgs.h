#pragma once

#include "Signal.h"
#include "Json.h"



namespace SignalArgs
{
	class Base
	{
	public:

		Signal::Target Target = Signal::Target::Unknown;
		int Action = -1; // enum Action
		int ViewId = -1;
		bool Valid = false;

	public:

		//:WARING - delete pointer after use
		static Base* CreateInstance(Json::Object& content);

		Base() {}

		Base(Json::Object& content);

		~Base();

		virtual CString Dump();
	};



	class Command : public Base
	{
	public:

		CString GroupName;
		CString GlobalName;
		Json::Object Options;

	public:

		Command(Json::Object& content);

		CString Dump() override;
	};



	class Initialize : public Base
	{
	public:

		HWND hWnd = nullptr;
		CString FilePath;

	public:

		Initialize(Json::Object& content);

		CString Dump() override;
	};



	class Mouse : public Base
	{
	public:

		UINT Flags = 0;
		int X = INT_MIN;
		int Y = INT_MIN;
		int Delta = 0; // wheel

	public:

		Mouse(Json::Object& content);

		CString Dump() override;

	public:

		bool ControlKey();
		bool ShiftKey();
		bool AltKey();

		bool LeftButton();
		bool MiddleButton();
		bool RightButton();
	};



	class Paint : public Base
	{
	public:

		// Renderer::PostPaintSignal()
		Paint(int viewId);

		Paint(Json::Object& content);

	public:

		CString Dump() override;
	};



	class Resize : public Base
	{
	public:

		int Width = 0;
		int Height = 0;

	public:

		Resize(Json::Object& content);

		CString Dump() override;
	};



	class Text : public Base
	{
	public:

		CString Buffer;

	public:

		Text(Json::Object& content);

	public:

		CString Dump() override;

	public:

		COLORREF ToColor();

		double ToDouble();

		int ToInteger();

		CPoint ToPoint();
	};
}