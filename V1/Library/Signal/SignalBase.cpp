#include "stdafx.h"
#include "SignalBase.h"
#include "WStr.h"

#define DUMP_BASE
//#define DUMP_MOUSE



namespace Signal
{
#pragma region Base Class

	Base* Base::CreateInstance(Json::Object& content)
	{
		Base* instance = nullptr;
		Signal::Target target = (Signal::Target)content.GetInteger(SKW_TARGET, -1);

		if (target == Signal::Target::View) {
			Signal::View::Action action = (Signal::View::Action)content.GetInteger(SKW_ACTION, -1);

			switch (action) {
			case Signal::View::Action::OnMouseMove:
			case Signal::View::Action::OnLButtonDown:
			case Signal::View::Action::OnLButtonUp:
			case Signal::View::Action::OnMButtonDown:
			case Signal::View::Action::OnMButtonUp:
			case Signal::View::Action::OnRButtonDown:
			case Signal::View::Action::OnRButtonUp:
			case Signal::View::Action::OnMouseWheel:
				instance = new MouseEvent(content);
				break;

			case Signal::View::Action::OnPaint:
				instance = new ResizeEvent(content);
				break;

			case Signal::View::Action::Unknown:
			default:
				DEBUG_STOP;
				break;
			}
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
		Valid = (ViewId > -1);
		ASSERT(Valid);
	}



	Base::~Base()
	{
	}



	CString Base::Dump()
	{
		CString serializer(L"Base, ");
		serializer += WStr::Format(L"Target: %d, ", (int)Target);
		serializer += WStr::Format(L"Action: %d, ", Action);
		serializer += WStr::Format(L"ViewId: %d, ", ViewId);
		serializer += WStr::Format(L"Valid: %s, ", WStr::ToString(Valid));

	#ifdef DUMP_BASE
		return serializer;
	#else
		return CString();
	#endif
	}

#pragma endregion //:REGION

#pragma region MouseEvent Class

	MouseEvent::MouseEvent(Json::Object& content)
		: Base(content)
	{
		if (Valid == false) {
			return;
		}

		Flags = content.GetInteger(SKW_FLAG);
		X = content.GetInteger(SKW_X, INT_MIN);
		Y = content.GetInteger(SKW_Y, INT_MIN);
		Delta = content.GetInteger(SKW_DELTA, 0);

		Valid &= (X > INT_MIN && Y > INT_MIN);
	}



	CString MouseEvent::Dump()
	{
		CString serializer(L"\nMouseEvent, ");
		serializer += WStr::Format(L"Flags: %d, ", Flags);
		serializer += WStr::Format(L"X: %d, ", X);
		serializer += WStr::Format(L"Y: %d, ", Y);
		serializer += WStr::Format(L"Delta: %d, ", Delta);
		serializer += __super::Dump();

	#ifdef DUMP_MOUSE
		return serializer;
	#else
		return CString();
	#endif
	}



	bool MouseEvent::ControlKey()
	{
		return Flags & MK_CONTROL;
	}

	bool MouseEvent::ShiftKey()
	{
		return Flags & MK_SHIFT;
	}

	bool MouseEvent::AltKey()
	{
		return Flags & MK_ALT;
	}

	bool MouseEvent::LeftButton()
	{
		return Flags & MK_LBUTTON;
	}

	bool MouseEvent::MiddleButton()
	{
		return Flags & MK_MBUTTON;
	}

	bool MouseEvent::RightButton()
	{
		return Flags & MK_RBUTTON;
	}

#pragma endregion //:REGION

#pragma region ResizeEvent Class

	ResizeEvent::ResizeEvent(Json::Object& content)
		: Base(content)
	{
		Width = content.GetInteger(SKW_WIDTH);
		Height = content.GetInteger(SKW_HEIGHT);
		Valid &= (Width > 0 && Height > 0);
	}



	CString ResizeEvent::Dump()
	{
		CString serializer(L"\nResizeEvent, ");
		serializer += WStr::Format(L"Width: %d, ", Width);
		serializer += WStr::Format(L"Height: %d, ", Height);
		serializer += __super::Dump();

		return serializer;
	}

#pragma endregion //:REGION
}