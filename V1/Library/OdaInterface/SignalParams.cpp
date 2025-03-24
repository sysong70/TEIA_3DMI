#include "stdafx.h"

#include "SignalParams.h"

#include "Json.h"
#include "WStr.h"
#include <atltypes.h>

#define DUMP_BASE
//#define DUMP_COMMAND
#define DUMP_INITIALIZE
#define DUMP_KEYBOARD
#define DUMP_MOUSE
#define DUMP_RESIZE
#define DUMP_PAINT
//#define DUMP_TEXT

//**************************************************************************************************

#pragma region SignalParams Class

SignalParams* SignalParams::CreateInstance(Json::Object& content, int& signalTarget, int& signalAction)
{
#define OnAction(x) SgnView::Action::On##x: instance = new x##Signal(content); break

	SignalParams* instance = nullptr;
	signalTarget = content.GetInteger(SKW_TARGET, -1);
	signalAction = content.GetInteger(SKW_ACTION, -1);

	Sgn::ETarget target = (Sgn::ETarget)signalTarget;

	if (target == Sgn::ETarget::View) {
		SgnView::Action action = (SgnView::Action)signalAction;

		switch (action) {
		case SgnView::Action::OnLButtonDown:
		case SgnView::Action::OnLButtonUp:
		case SgnView::Action::OnMButtonDown:
		case SgnView::Action::OnMButtonUp:
		case SgnView::Action::OnRButtonDown:
		case SgnView::Action::OnRButtonUp:
		case SgnView::Action::OnMouseMove:
		case SgnView::Action::OnMouseWheel:
			instance = new MouseSignal(content);
			break;

		case OnAction(Paint);
		case OnAction(Resize);
		case OnAction(Initialize);
		case OnAction(Command);
		case OnAction(ContextCommand);

		case SgnView::Action::OnKeyDown:
			//instance = new Keyboard(content);
			break;

		case SgnView::Action::OnConstruct:	TRACE(L"TODO - SgnView::Action::OnConstruct"); break;
		case SgnView::Action::OnDestruct:	TRACE(L"TODO - SgnView::Action::OnDestruct"); break;
		case SgnView::Action::OnCancel:		TRACE(L"TODO - SgnView::Action::OnCancel"); break;

		case SgnView::Action::Unknown:
		default:
			DEBUG_STOP;
			break;
		}
	}
	else if (target == Sgn::ETarget::Application) {
		SgnApplication::Action action = (SgnApplication::Action)signalAction;

		switch (action) {
		case SgnApplication::Action::OnInitInstance:		TRACE(L"TODO - SgnApplication::Action::OnInitInstance"); break;
		case SgnApplication::Action::OnUpdatePreference:	TRACE(L"TODO - SgnApplication::Action::OnUpdatePreference"); break;
		case SgnApplication::Action::OnUpdateFileOption:	TRACE(L"TODO - SgnApplication::Action::OnUpdateFileOption"); break;
		case SgnApplication::Action::OnExitInstance:		TRACE(L"TODO - SgnApplication::Action::OnExitInstance"); break;
		case SgnApplication::Action::OnDpiAware:			TRACE(L"TODO - SgnApplication::Action::OnDpiAware"); break;

		default:
			DEBUG_STOP;
			break;
		};
	}
	else if (target == Sgn::ETarget::TaskBar) {
	}
	else if (target == Sgn::ETarget::UserIO) {
		SgnUserIO::Action action = (SgnUserIO::Action)signalAction;

		switch (action) {
		case SgnUserIO::Action::OnInput:
			instance = new TextSignal(content);
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



SignalParams::SignalParams(Json::Object& content)
{
	Target = (Sgn::ETarget)content.GetInteger(SKW_TARGET, -1);
	Action = content.GetInteger(SKW_ACTION, -1);
	ViewId = content.GetInteger(SKW_VIEWID, -1);

	Valid = (Target != Sgn::ETarget::Unknown && Action != -1 && ViewId >= 0);
	ASSERT(Valid);
}



SignalParams::~SignalParams()
{
}



CString SignalParams::Dump()
{
	CString serializer(L"[BaseSignal] ");
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

#pragma region CommandSignal Class

CommandSignal::CommandSignal(Json::Object& content)
	: SignalParams(content)
{
	Id = content.GetInteger(SKW_ID, -1);
	if (Json::Value* pValue = content.FindValue(SKW_OPTIONS)) {
		Options = pValue->AsObject();
	}

	Valid &= (Id > 0);
	ASSERT(Valid);
}



CString CommandSignal::Dump()
{
	CString serializer(L"\n[CommandSignal] ");
	serializer += WStr::Format(L"Id:%d, ", Id);
	serializer += __super::Dump();

#ifdef DUMP_COMMAND
	return serializer;
#else
	return CString();
#endif
}

#pragma endregion //:REGION

#pragma region ContextCommandSignal Class

ContextCommandSignal::ContextCommandSignal(Json::Object& content)
	: CommandSignal(content)
{
}

#pragma endregion //:REGION

#pragma region InitializeSignal Class

InitializeSignal::InitializeSignal(Json::Object& content)
	: SignalParams(content)
{
	WindowHandle = (HWND)content.GetDwordPtr(SKW_HWND);
	FilePath = content.GetString(SKW_FILEPATH);

	Valid &= (WindowHandle != nullptr && FilePath.IsEmpty() == false);
	ASSERT(Valid);
}



CString InitializeSignal::Dump()
{
	CString serializer(L"\n[InitializeSignal] ");
	serializer += WStr::Format(L"HWND:%lld, ", WindowHandle);
	serializer += __super::Dump();

#ifdef DUMP_INITIALIZE
	return serializer;
#else
	return CString();
#endif
}

#pragma endregion //:REGION

#pragma region KeyboardSignal Class

KeyboardSignal::KeyboardSignal(Json::Object& content)
	: SignalParams(content)
{
	Flags = content.GetInteger(SKW_FLAG);
	Repeat = content.GetInteger(SKW_REPCNT);
	Char = content.GetInteger(SKW_CHAR);
	Down = static_cast<SgnView::Action>(Action) == SgnView::Action::OnKeyDown;
}



CString KeyboardSignal::Dump()
{
	CString serializer(L"\n[KeyboardSignal] ");
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

#pragma endregion //:REGION

#pragma region MouseSignal Class

MouseSignal::MouseSignal(Json::Object& content)
	: SignalParams(content)
{
	Flags = content.GetInteger(SKW_FLAG);
	X = content.GetInteger(SKW_X, INT_MIN);
	Y = content.GetInteger(SKW_Y, INT_MIN);
	Delta = content.GetInteger(SKW_DELTA, 0);

	Valid &= (X > INT_MIN && Y > INT_MIN);
	ASSERT(Valid);
}



CPoint MouseSignal::GetPoint()
{
	return CPoint(X, Y);
}



CString MouseSignal::Dump()
{
	CString serializer(L"\n[MouseSignal] ");
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

#pragma endregion //:REGION

#pragma region PaintSignal Class

PaintSignal::PaintSignal(Json::Object& content)
	: SignalParams(content)
{
	Json::Value* pValue = content.FindValue(SKW_VALUE);
	if (pValue != nullptr) {
		Json::Array& rect = pValue->AsArray();

		Left = rect.GetInteger(0);
		Top = rect.GetInteger(1);
		Right = rect.GetInteger(2);
		Bottom = rect.GetInteger(3);
	}
}



PaintSignal::PaintSignal(int viewId)
	: SignalParams()
{
	Target = Sgn::ETarget::View;
	Action = (int)SgnView::Action::OnPaint;
	ViewId = viewId;

	Valid = (ViewId > -1);
	ASSERT(Valid);
}



CString PaintSignal::Dump()
{
	CString serializer(L"\n[PaintSignal] ");
	serializer += WStr::Format(L"Left:%d, ", Left);
	serializer += WStr::Format(L"Top:%d, ", Top);
	serializer += WStr::Format(L"Right:%d, ", Right);
	serializer += WStr::Format(L"Bottom:%d, ", Bottom);

	Options.Serialize(serializer, 4);

	serializer += __super::Dump();

#ifdef DUMP_PAINT
	return serializer;
#else
	return CString();
#endif
}

#pragma endregion //:REGION

#pragma region ResizeSignal Class

ResizeSignal::ResizeSignal(Json::Object& content)
	: SignalParams(content)
{
	Width = content.GetInteger(SKW_X);
	Height = content.GetInteger(SKW_Y);

	Valid &= (Width > 0 && Height > 0);
	ASSERT(Valid);
}



CString ResizeSignal::Dump()
{
	CString serializer(L"\n[ResizeSignal] ");
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

#pragma region TextSignal Class

TextSignal::TextSignal(Json::Object& content)
	: SignalParams(content)
{
	Buffer = content.GetString(SKW_VALUE);
}



CString TextSignal::Dump()
{
	CString serializer(L"\n[TextSignal] ");
	serializer += WStr::Format(L"Buffer:%s, ", Buffer);
	serializer += __super::Dump();

#ifdef DUMP_TEXT
	return serializer;
#else
	return CString();
#endif
}

#pragma endregion //:REGION