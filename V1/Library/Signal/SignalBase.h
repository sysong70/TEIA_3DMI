#pragma once

#include "Signal.h"
#include "Json.h"



namespace Signal
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

	public:

		virtual CString Dump();
	};



	class MouseEvent : public Base
	{
	public:

		UINT Flags = 0;
		int X = INT_MIN;
		int Y = INT_MIN;
		int Delta = 0; // wheel

	public:

		MouseEvent(Json::Object& content);

	public:

		CString Dump() override;

	public:

		bool ControlKey();
		bool ShiftKey();
		bool AltKey();

		bool LeftButton();
		bool MiddleButton();
		bool RightButton();
	};



	class ResizeEvent : public Base
	{
	public:

		int Width = 0;
		int Height = 0;

	public:

		ResizeEvent(Json::Object& content);

	public:

		CString Dump() override;
	};
}