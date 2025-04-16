#include "StdAfx.h"

#include "Operator.Event.h"
#include "Point.h"

#include "3DF.Utility.h"

#include "Window.h"

#include <HEventInfo.h>

using namespace H3DF;

// HEventInfo와 연계성을 위해서 KERNEL::Command::Event를 보조하기 위해서 H3DF::Operator::Event로 생성함.

//== Event class ===============================================================================
namespace H3DF
{
	namespace Operator
	{
		class EventImpl : public Impl
		{
		public:
			void Copy(EventImpl * pcInThat)
			{
				m_cWindowKey = pcInThat->m_cWindowKey;
				m_eEventType = pcInThat->m_eEventType;
				m_nFlags = pcInThat->m_nFlags;
				m_nWheelDelta = pcInThat->m_nWheelDelta;

				m_cPixelPoint = pcInThat->m_cPixelPoint;
				m_cWindowPoint = pcInThat->m_cWindowPoint;
				m_cWorldPoint = pcInThat->m_cWorldPoint;
			}

			void SetType(Event::Type eInType) { m_eEventType = eInType; }

			H3DF::WindowKey m_cWindowKey;
			Event::Type m_eEventType = Event::Type::NoEvent;
			UINT m_nFlags = 0;
			int m_nWheelDelta = 0;

			PixelPoint m_cPixelPoint;
			WindowPoint m_cWindowPoint;
			WorldPoint m_cWorldPoint;
		};
	}
}

H3DF::Operator::Event::Event()
{
	m_pcImpl = new EventImpl();
	DEBUG_VALID(m_pcImpl);
}

H3DF::Operator::Event::Event(H3DF::WindowKey & cInWindowKey)
{
	EventImpl * pcImpl = new EventImpl();
	DEBUG_VALID(pcImpl);

	pcImpl->m_cWindowKey = cInWindowKey;

	m_pcImpl = pcImpl;
}

H3DF::Operator::Event::Event(Event const & cInThat)
{
	m_pcImpl = new EventImpl();
	DEBUG_VALID(m_pcImpl);
	Set(cInThat);
}

void H3DF::Operator::Event::Set(Event const & cInThat)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	EventImpl * pcInThatImpl = (EventImpl *) cInThat.m_pcImpl;

	pcImpl->Copy(pcInThatImpl);
}

H3DF::Operator::Event & H3DF::Operator::Event::operator = (Event const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::Operator::Event::SetWindow(H3DF::WindowKey & cInWindowKey)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cWindowKey = cInWindowKey;
}

void H3DF::Operator::Event::SetPoint(Operator::Event::Type eInType, int x, int y, UINT nInFlags)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_eEventType = eInType;
	pcImpl->m_nFlags = nInFlags;

	pcImpl->m_cPixelPoint.x = (float) x;
	pcImpl->m_cPixelPoint.y = (float) y;
	pcImpl->m_cPixelPoint.z = 0.0f;

	pcImpl->m_cWorldPoint = WorldPoint(pcImpl->m_cWindowKey, pcImpl->m_cPixelPoint);

	pcImpl->m_cWindowPoint = WindowPoint(pcImpl->m_cWindowKey, pcImpl->m_cPixelPoint);
	pcImpl->m_cWindowPoint.z = 0.0f;
	pcImpl->m_cWindowPoint.ClampPoint();
}

int H3DF::Operator::Event::GetMouseWheelDelta()
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_nWheelDelta;
}

void H3DF::Operator::Event::SetMouseWheelDelta(int nInDelta)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nWheelDelta = nInDelta;
}

bool H3DF::Operator::Event::Control() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->m_nFlags & (UINT) Event::Flag::Control);
}

bool H3DF::Operator::Event::Shift() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->m_nFlags & (UINT) Event::Flag::Shift);
}
bool H3DF::Operator::Event::Alt() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->m_nFlags & (UINT) Event::Flag::Alt);
}

bool H3DF::Operator::Event::LButton() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->m_nFlags & (UINT) Event::Flag::LeftButton);
}

bool H3DF::Operator::Event::MButton() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->m_nFlags & (UINT) Event::Flag::MiddleButton);
}

bool H3DF::Operator::Event::RButton() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return 0 != (pcImpl->m_nFlags & (UINT) Event::Flag::RightButton);
}

PixelPoint const & H3DF::Operator::Event::GetMousePixelPoint() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cPixelPoint;
}

WindowPoint const & H3DF::Operator::Event::GetMouseWindowPoint() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cWindowPoint;
}

WorldPoint const & H3DF::Operator::Event::GetMouseWorldPoint() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cWorldPoint;
}

void H3DF::Operator::Event::SetEventType(Event::Type cInType)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_eEventType = cInType;
}

H3DF::Operator::Event::Type H3DF::Operator::Event::GetEventType() const
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eEventType;
}


bool H3DF::Operator::Event::GetHEventInfo(HEventInfo & cOutEvent)
{
	EventImpl * pcImpl = (EventImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	HEventType cEventType = HE_NoEvent;

	switch (pcImpl->m_eEventType)
	{
		case Operator::Event::Type::NoEvent:
			cEventType = HE_NoEvent;
			break;

		case Operator::Event::Type::LButtonDown:
			cEventType = HE_LButtonDown;
			break;

		case Operator::Event::Type::LButtonUp:
			cEventType = HE_LButtonUp;
			break;

		case Operator::Event::Type::LButtonDblClk:
			cEventType = HE_LButtonDblClk;
			break;

		case Operator::Event::Type::MButtonDown:
			cEventType = HE_MButtonDown;
			break;

		case Operator::Event::Type::MButtonUp:
			cEventType = HE_MButtonUp;
			break;

		case Operator::Event::Type::MButtonDblClk:
			cEventType = HE_MButtonDblClk;
			break;

		case Operator::Event::Type::RButtonDown:
			cEventType = HE_RButtonDown;
			break;

		case Operator::Event::Type::RButtonUp:
			cEventType = HE_RButtonUp;
			break;

		case Operator::Event::Type::RButtonDblClk:
			cEventType = HE_RButtonDblClk;
			break;

		case Operator::Event::Type::MouseMove:
			cEventType = HE_MouseMove;
			break;

		case Operator::Event::Type::MouseWheel:
			cEventType = HE_MouseWheel;
			break;

		case Operator::Event::Type::MouseWheelUp:
			cEventType = HE_MouseWheelUp;
			break;

		case Operator::Event::Type::MouseWheelDown:
			cEventType = HE_MouseWheelDown;
			break;

		case Operator::Event::Type::Timer:
			cEventType = HE_Timer;
			break;

		case Operator::Event::Type::KeyDown:
			cEventType = HE_KeyDown;
			break;

		case Operator::Event::Type::KeyUp:
			cEventType = HE_KeyUp;
			break;

		case Operator::Event::Type::Selection:
			cEventType = HE_Selection;
			break;

		case Operator::Event::Type::JoyStickTranslate:
			cEventType = HE_JoyStickTranslate;
			break;

		case Operator::Event::Type::JoyStickRotate:
			cEventType = HE_JoyStickRotate;
			break;

		case Operator::Event::Type::JoyStickButton:
			cEventType = HE_JoyStickButton;
			break;

		case Operator::Event::Type::TouchesDown:
			cEventType = HE_TouchesDown;
			break;

		case Operator::Event::Type::TouchesUp:
			cEventType = HE_TouchesUp;
			break;

		case Operator::Event::Type::TouchesMove:
			cEventType = HE_TouchesMove;
			break;
	}

	cOutEvent.SetType(cEventType);

	int nFlags = 0;

	switch ((Operator::Event::Flag)pcImpl->m_nFlags)
	{
		case Operator::Event::Flag::Control:
			nFlags = MVO_CONTROL;
			break;

		case Operator::Event::Flag::Shift:
			nFlags = MVO_SHIFT;
			break;

		case Operator::Event::Flag::Alt:
			nFlags = MVO_ALT;
			break;

		case Operator::Event::Flag::LeftButton:
			nFlags = MVO_LBUTTON;
			break;

		case Operator::Event::Flag::MiddleButton:
			nFlags = MVO_MBUTTON;
			break;

		case Operator::Event::Flag::RightButton:
			nFlags = MVO_RBUTTON;
			break;

		case Operator::Event::Flag::RightControl:
			nFlags = MVO_RIGHT_CONTROL;
			break;

		case Operator::Event::Flag::RightShift:
			nFlags = MVO_RIGHT_SHIFT;
			break;

		case Operator::Event::Flag::RightAlt:
			nFlags = MVO_RIGHT_ALT;
			break;

		case Operator::Event::Flag::LeftControl:
			nFlags = MVO_LEFT_CONTROL;
			break;

		case Operator::Event::Flag::LeftShift:
			nFlags = MVO_LEFT_SHIFT;
			break;

		case Operator::Event::Flag::LeftAlt:
			nFlags = MVO_LEFT_ALT;
			break;

		case Operator::Event::Flag::LeftArrow:
			nFlags = MVO_LEFT_ARROW;
			break;

		case Operator::Event::Flag::UpArrow:
			nFlags = MVO_UP_ARROW;
			break;

		case Operator::Event::Flag::RightArrow:
			nFlags = MVO_RIGHT_ARROW;
			break;

		case Operator::Event::Flag::DownArrow:
			nFlags = MVO_DOWN_ARROW;
			break;

		case Operator::Event::Flag::JoystickButton1:
			nFlags = MVO_JOYSTICK_BUTTON_1;
			break;

		case Operator::Event::Flag::JoystickButton2:
			nFlags = MVO_JOYSTICK_BUTTON_2;
			break;

		case Operator::Event::Flag::JoystickButton3:
			nFlags = MVO_JOYSTICK_BUTTON_3;
			break;

		case Operator::Event::Flag::JoystickButton4:
			nFlags = MVO_JOYSTICK_BUTTON_4;
			break;

		case Operator::Event::Flag::JoystickButton5:
			nFlags = MVO_JOYSTICK_BUTTON_5;
			break;

		case Operator::Event::Flag::JoystickButton6:
			nFlags = MVO_JOYSTICK_BUTTON_6;
			break;

		case Operator::Event::Flag::JoystickButton7:
			nFlags = MVO_JOYSTICK_BUTTON_7;
			break;

		case Operator::Event::Flag::JoystickButton8:
			nFlags = MVO_JOYSTICK_BUTTON_8;
			break;
	}

	cOutEvent.SetFlags(nFlags);

	cOutEvent.SetMouseWheelDelta(pcImpl->m_nWheelDelta);

	HPoint cPoint;

	cPoint = H3DF::Utility::ToHPoint(pcImpl->m_cPixelPoint);
	*(HPoint *)&cOutEvent.GetMousePixelPos() = cPoint;

	cPoint = H3DF::Utility::ToHPoint(pcImpl->m_cWindowPoint);
	*(HPoint *) &cOutEvent.GetMouseWindowPos() = cPoint;

	cPoint = H3DF::Utility::ToHPoint(pcImpl->m_cWorldPoint);
	*(HPoint *) &cOutEvent.GetMouseWorldPos() = cPoint;

	return true;
}