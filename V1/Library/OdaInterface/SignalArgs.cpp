#include "stdafx.h"
#include "SignalArgs.h"
#include "WStr.h"

#define DUMP_BASE
//#define DUMP_COMMAND
#define DUMP_INITIALIZE
//#define DUMP_MOUSE
#define DUMP_RESIZE
#define DUMP_PAINT
//#define DUMP_TEXT



namespace SignalArgs
{
#pragma region Base Class

	Base* Base::CreateInstance(Json::Object& content)
	{
		Base* instance = nullptr;
		Signal::Target target = (Signal::Target)content.GetInteger(SKW_TARGET, -1);

		if (target == Signal::Target::View) {
			Signal::View::Action action = (Signal::View::Action)content.GetInteger(SKW_ACTION, -1);

			switch (action) {
			case Signal::View::Action::OnLButtonDown:
			case Signal::View::Action::OnLButtonUp:
			case Signal::View::Action::OnMButtonDown:
			case Signal::View::Action::OnMButtonUp:
			case Signal::View::Action::OnRButtonDown:
			case Signal::View::Action::OnRButtonUp:
			case Signal::View::Action::OnMouseMove:
			case Signal::View::Action::OnMouseWheel:
				instance = new Mouse(content);
				break;

			case Signal::View::Action::OnPaint:
				instance = new Paint(content);
				break;

			case Signal::View::Action::OnResize:
				instance = new Resize(content);
				break;

			case Signal::View::Action::OnConstruct:
			case Signal::View::Action::OnDestruct:
				break;

			case Signal::View::Action::OnInitialize:
				instance = new Initialize(content);
				break;

			case Signal::View::Action::Unknown:
			default:
				DEBUG_STOP;
				break;
			}
		}
		else if (target == Signal::Target::Application) {
			Signal::Application::Action action = (Signal::Application::Action)content.GetInteger(SKW_ACTION, -1);
			ASSERT(action == Signal::Application::Action::OnInitInstance);
		}
		else {
			DEBUG_STOP;
		}

		return instance;
	}



	Base::Base(Json::Object& content)
	{
		Target = (Signal::Target)content.GetInteger(SKW_TARGET, -1);
		Action = content.GetInteger(SKW_ACTION, -1);
		ViewId = content.GetInteger(SKW_VIEWID, -1);

		Valid = (Target != Signal::Target::Unknown && Action != -1 && ViewId >= 0);
		ASSERT(Valid);
	}



	Base::~Base()
	{
	}



	CString Base::Dump()
	{
		CString serializer(L"[Base] ");
		serializer += WStr::Format(L"Target:%d, ", (int)Target);
		serializer += WStr::Format(L"Action:%d, ", Action);
		serializer += WStr::Format(L"ViewId:%d, ", ViewId);
		serializer += WStr::Format(L"Valid:%s", WStr::ToString(Valid));

	#ifdef DUMP_BASE
		return serializer;
	#else
		return CString();
	#endif
	}

#pragma endregion //:REGION

#pragma region Command Class

	Command::Command(Json::Object& content)
		: Base(content)
	{
		GroupName = content.GetString(SKW_GROUPNAME);
		GlobalName = content.GetString(SKW_GLOBALNAME);
		Options = content.GetAt(SKW_OPTION);

		Valid &= (GlobalName.IsEmpty() == false);
		ASSERT(Valid);
	}



	CString Command::Dump()
	{
		CString serializer(L"\n[Command] ");
		serializer += WStr::Format(L"GroupName:%s, ", GroupName);
		serializer += WStr::Format(L"GlobalName:%s, ", GlobalName);
		serializer += WStr::Format(L"Options:%s, ", Options.ToString());
		serializer += __super::Dump();

	#ifdef DUMP_COMMAND
		return serializer;
	#else
		return CString();
	#endif
	}

#pragma endregion //:REGION

#pragma region Initialize Class

	Initialize::Initialize(Json::Object& content)
		: Base(content)
	{
		hWnd = (HWND)content.GetDwordPtr(SKW_HWND);
		FilePath = content.GetString(SKW_FILEPATH);

		Valid &= (hWnd != nullptr && FilePath.IsEmpty() == false);
		ASSERT(Valid);
	}



	CString Initialize::Dump()
	{
		CString serializer(L"\n[Initialize] ");
		serializer += WStr::Format(L"HWND:%lld, ", hWnd);
		serializer += __super::Dump();

	#ifdef DUMP_INITIALIZE
		return serializer;
	#else
		return CString();
	#endif
	}

#pragma region Mouse Class

	Mouse::Mouse(Json::Object& content)
		: Base(content)
	{
		Flags = content.GetInteger(SKW_FLAG);
		X = content.GetInteger(SKW_X, INT_MIN);
		Y = content.GetInteger(SKW_Y, INT_MIN);
		Delta = content.GetInteger(SKW_DELTA, 0);

		Valid &= (X > INT_MIN && Y > INT_MIN);
		ASSERT(Valid);
	}



	CString Mouse::Dump()
	{
		CString serializer(L"\n[Mouse] ");
		serializer += WStr::Format(L"Flags:%d, ", Flags);
		serializer += WStr::Format(L"X:%d, ", X);
		serializer += WStr::Format(L"Y:%d, ", Y);
		serializer += WStr::Format(L"Delta:%d, ", Delta);
		serializer += __super::Dump();

	#ifdef DUMP_MOUSE
		return serializer;
	#else
		return CString();
	#endif
	}



	bool Mouse::ControlKey()
	{
		return Flags & MK_CONTROL;
	}

	bool Mouse::ShiftKey()
	{
		return Flags & MK_SHIFT;
	}

	bool Mouse::AltKey()
	{
		return Flags & MK_ALT;
	}

	bool Mouse::LeftButton()
	{
		return Flags & MK_LBUTTON;
	}

	bool Mouse::MiddleButton()
	{
		return Flags & MK_MBUTTON;
	}

	bool Mouse::RightButton()
	{
		return Flags & MK_RBUTTON;
	}

#pragma endregion //:REGION

#pragma region Paint Class

	Paint::Paint(int viewId)
		: Base()
	{
		Target = Signal::Target::View;
		Action = (int)Signal::View::Action::OnPaint;
		ViewId = viewId;

		Valid = (ViewId > -1);
		ASSERT(Valid);
	}

	Paint::Paint(Json::Object& content)
		: Base(content)
	{
	}



	CString Paint::Dump()
	{
		CString serializer(L"\n[Paint] ");
		serializer += __super::Dump();

	#ifdef DUMP_PAINT
		return serializer;
	#else
		return CString();
	#endif
	}

#pragma endregion //:REGION

#pragma region Resize Class

	Resize::Resize(Json::Object& content)
		: Base(content)
	{
		Width = content.GetInteger(SKW_X);
		Height = content.GetInteger(SKW_Y);

		Valid &= (Width > 0 && Height > 0);
		ASSERT(Valid);
	}



	CString Resize::Dump()
	{
		CString serializer(L"\n[Resize] ");
		serializer += WStr::Format(L"Width:%d, ", Width);
		serializer += WStr::Format(L"Height:%d, ", Height);
		serializer += __super::Dump();

	#ifdef DUMP_RESIZE
		return serializer;
	#else
		return CString();
	#endif
	}

#pragma endregion //:REGION

#pragma region Text Class

	Text::Text(Json::Object& content)
		: Base(content)
	{
		DEBUG_STOP;
	}



	CString Text::Dump()
	{
		CString serializer(L"\n[Text] ");
		serializer += WStr::Format(L"Buffer:%s, ", Buffer);
		serializer += __super::Dump();

	#ifdef DUMP_TEXT
		return serializer;
	#else
		return CString();
	#endif
	}



	COLORREF Text::ToColor()
	{
		return COLORREF();
	}



	int Text::ToInteger()
	{
		return 0;
	}



	CPoint Text::ToPoint()
	{
		return CPoint();
	}



	double Text::ToDouble()
	{
		return 0.0;
	}

#pragma endregion
}