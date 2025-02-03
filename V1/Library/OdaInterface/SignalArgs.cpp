#include "stdafx.h"
#include "SignalArgs.h"
#include "WStr.h"

#define DUMP_BASE
//#define DUMP_COMMAND
#define DUMP_INITIALIZE
#define DUMP_KEYBOARD
#define DUMP_MOUSE
#define DUMP_RESIZE
#define DUMP_PAINT
//#define DUMP_TEXT

//**************************************************************************************************

#pragma region Base Class

SignalArgs::Base* SignalArgs::Base::CreateInstance(Json::Object& content, int& signalTarget, int& signalAction)
{
#define OnAction(x) Signal::View::Action::On##x: instance = new x(content); break

	Base* instance = nullptr;
	signalTarget = content.GetInteger(SKW_TARGET, -1);
	signalAction = content.GetInteger(SKW_ACTION, -1);

	Signal::Target target = (Signal::Target)signalTarget;

	if (target == Signal::Target::View) {
		Signal::View::Action action = (Signal::View::Action)signalAction;

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

		case OnAction(Paint);
		case OnAction(Resize);
		case OnAction(Initialize);
		case OnAction(Command);
		case OnAction(ContextCommand);

		case Signal::View::Action::OnKeyDown:
			instance = new Keyboard(content);
			break;

		case Signal::View::Action::OnConstruct:
			TRACE(L"TODO - Signal::View::Action::OnConstruct");
			break;
		case Signal::View::Action::OnDestruct:
			TRACE(L"TODO - Signal::View::Action::OnDestruct");
			break;
		case Signal::View::Action::OnCancel:
			TRACE(L"TODO - Signal::View::Action::OnCancel");
			break;

		case Signal::View::Action::Unknown:
		default:
			DEBUG_STOP;
			break;
		}
	}
	else if (target == Signal::Target::Application) {
		Signal::Application::Action action = (Signal::Application::Action)signalAction;

		switch (action) {
		case Signal::Application::Action::OnInitInstance: break;
		case Signal::Application::Action::OnUpdatePreference: break;
		case Signal::Application::Action::OnUpdateFileOption: break; 
		case Signal::Application::Action::OnExitInstance: break;
		case Signal::Application::Action::OnDpiAware: break;

		default:
			DEBUG_STOP;
			break;
		};
	}
	else if (target == Signal::Target::TaskBar) {
	}
	else if (target == Signal::Target::UserIO) {
		Signal::UserIO::Action action = (Signal::UserIO::Action)signalAction;

		switch (action) {
		case Signal::UserIO::Action::OnInput:
			instance = new Text(content);
			break;

		default:
			DEBUG_STOP;
			break;
		}
	}
	else {
		DEBUG_STOP;
	}

	return instance;

#undef OnAction
}



SignalArgs::Base::Base(Json::Object& content)
{
	Target = (Signal::Target)content.GetInteger(SKW_TARGET, -1);
	Action = content.GetInteger(SKW_ACTION, -1);
	ViewId = content.GetInteger(SKW_VIEWID, -1);

	Valid = (Target != Signal::Target::Unknown && Action != -1 && ViewId >= 0);
	ASSERT(Valid);
}



SignalArgs::Base::~Base()
{
}



CString SignalArgs::Base::Dump()
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

#pragma endregion // REGION

#pragma region Command Class

SignalArgs::Command::Command(Json::Object& content)
	: Base(content)
{
	Id = content.GetInteger(SKW_ID, -1);

	Valid &= (Id > 0);
	ASSERT(Valid);
}



CString SignalArgs::Command::Dump()
{
	CString serializer(L"\n[Command] ");
	serializer += WStr::Format(L"Id:%d, ", Id);
	serializer += __super::Dump();

#ifdef DUMP_COMMAND
	return serializer;
#else
	return CString();
#endif
}

#pragma endregion // REGION

#pragma region ContextCommand Class

SignalArgs::ContextCommand::ContextCommand(Json::Object& content)
	: Command(content)
{
}

#pragma endregion // REGION

#pragma region Initialize Class

SignalArgs::Initialize::Initialize(Json::Object& content)
	: Base(content)
{
	hWnd = (HWND)content.GetDwordPtr(SKW_HWND);
	FilePath = content.GetString(SKW_FILEPATH);

	Valid &= (hWnd != nullptr && FilePath.IsEmpty() == false);
	ASSERT(Valid);
}



CString SignalArgs::Initialize::Dump()
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

#pragma endregion // REGION

#pragma region Keyboard Class

SignalArgs::Keyboard::Keyboard(Json::Object& content)
	: Base(content)
{
	Flags = content.GetInteger(SKW_FLAG);
	Repeat = content.GetInteger(SKW_REPCNT);
	Char = content.GetInteger(SKW_CHAR);
	Down = static_cast<Signal::View::Action>(Action) == Signal::View::Action::OnKeyDown;
}



bool SignalArgs::Keyboard::IsEnter()
{
	return Char == VK_RETURN;
}



bool SignalArgs::Keyboard::IsEscape()
{
	return Char == VK_ESCAPE;
}



CString SignalArgs::Keyboard::Dump()
{
	CString serializer(L"\n[Mouse] ");
	serializer += WStr::Format(L"Flags:%d, ", Flags);
	serializer += WStr::Format(L"Repeat:%d, ", Repeat);
	serializer += WStr::Format(L"Char:%d, ", Char);
	serializer += WStr::Format(L"Down:%d, ", Down);
	serializer += __super::Dump();

#ifdef DUMP_KEYBOARD
	return serializer;
#else
	return CString();
#endif
}

#pragma endregion // REGION

#pragma region Mouse Class

SignalArgs::Mouse::Mouse(Json::Object& content)
	: Base(content)
{
	Flags = content.GetInteger(SKW_FLAG);
	X = content.GetInteger(SKW_X, INT_MIN);
	Y = content.GetInteger(SKW_Y, INT_MIN);
	Delta = content.GetInteger(SKW_DELTA, 0);

	Valid &= (X > INT_MIN && Y > INT_MIN);
	ASSERT(Valid);
}



CPoint SignalArgs::Mouse::GetPoint()
{
	return CPoint(X, Y);
}



CString SignalArgs::Mouse::Dump()
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



bool SignalArgs::Mouse::ControlKey()
{
	return Flags & MK_CONTROL;
}

bool SignalArgs::Mouse::ShiftKey()
{
	return Flags & MK_SHIFT;
}

bool SignalArgs::Mouse::AltKey()
{
	return Flags & MK_ALT;
}

bool SignalArgs::Mouse::LeftButton()
{
	return Flags & MK_LBUTTON;
}

bool SignalArgs::Mouse::MiddleButton()
{
	return Flags & MK_MBUTTON;
}

bool SignalArgs::Mouse::RightButton()
{
	return Flags & MK_RBUTTON;
}

#pragma endregion // REGION

#pragma region Paint Class

SignalArgs::Paint::Paint(int viewId)
	: Base()
{
	Target = Signal::Target::View;
	Action = (int)Signal::View::Action::OnPaint;
	ViewId = viewId;

	Valid = (ViewId > -1);
	ASSERT(Valid);
}

SignalArgs::Paint::Paint(Json::Object& content)
	: Base(content)
{
	Json::Array& rect = content.GetArray(SKW_RECT);

	Left = rect.GetInteger(0);
	Top = rect.GetInteger(1);
	Right = rect.GetInteger(2);
	Bottom = rect.GetInteger(3);
}



CString SignalArgs::Paint::Dump()
{
	CString serializer(L"\n[Paint] ");
	serializer += WStr::Format(L"Left:%d, ", Left);
	serializer += WStr::Format(L"Top:%d, ", Top);
	serializer += WStr::Format(L"Right:%d, ", Right);
	serializer += WStr::Format(L"Bottom:%d, ", Bottom);
	serializer += __super::Dump();

#ifdef DUMP_PAINT
	return serializer;
#else
	return CString();
#endif
}

#pragma endregion // REGION

#pragma region Resize Class

SignalArgs::Resize::Resize(Json::Object& content)
	: Base(content)
{
	Width = content.GetInteger(SKW_X);
	Height = content.GetInteger(SKW_Y);

	Valid &= (Width > 0 && Height > 0);
	ASSERT(Valid);
}



CString SignalArgs::Resize::Dump()
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

#pragma endregion // REGION

#pragma region Text Class

SignalArgs::Text::Text(Json::Object& content)
	: Base(content)
{
	Buffer = content.GetString(SKW_VALUE);
}



CString SignalArgs::Text::Dump()
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



COLORREF SignalArgs::Text::ToColor()
{
	return COLORREF();
}



int SignalArgs::Text::ToInteger()
{
	return 0;
}



CPoint SignalArgs::Text::ToPoint()
{
	return CPoint();
}



double SignalArgs::Text::ToDouble()
{
	return 0.0;
}
