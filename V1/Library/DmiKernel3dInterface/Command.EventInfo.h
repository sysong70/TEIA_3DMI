#pragma once

#include "Object.h"

#include <3DF/Point.h>
#include <3DF/Window.h>

namespace KERNEL
{
	namespace Command
	{
		class EventInfo : public Object
		{
		public:
			enum class Type {
				NoEvent, // No event ooccurred.

				LButtonDown, // The left mouse button was pressed.
				LButtonUp, // The left mouse button is up.
				LButtonDblClk, // The left mouse button was double-clicked.

				MButtonDown, // The middle mouse button was pressed.
				MButtonUp, // The middle mouse button is up.
				MButtonDblClk, // The middle mouse button was double-clicked.

				RButtonDown, // The right mouse button was pressed.
				RButtonUp, // The right mouse button is up.
				RButtonDblClk, // The right mouse button was double-clicked.

				MouseMove, // The mouse cursor has moved.

				MouseWheel, // The mouse wheel has moved.
				MouseWheelUp, // The mouse wheel was scrolled up.
				MouseWheelDown, // The mouse wheel was scrolled down.

				Timer, // self-explanatory

				KeyDown, // A key on the keyboard was pressed.
				KeyUp, // A key on the keyboard was released.

				// Higher level
				Selection, // A general selection event.

				JoyStickTranslate, // A 3D Translation vector from a 3D Mouse/Joystick
				JoyStickRotate, // A set of Rotation values from a 3D Mouse/Joystick
				JoyStickButton, // A button press from a 3D Mouse/Joystick

				TouchesDown, //  A touch down event has occured.
				TouchesUp, // A touch up event has occured.
				TouchesMove // A touch move event has occured.
			};

			enum class Flag {
				Control = 1 << 1,
				Shift = 1 << 2,
				Alt = 1 << 3,
				LeftButton = 1 << 4,
				MiddleButton = 1 << 5,
				RightButton = 1 << 6,
				RightControl = 1 << 7,
				RightShift = 1 << 8,
				RightAlt = 1 << 9,
				LeftControl = 1 << 10,
				LeftShift = 1 << 11,
				LeftAlt = 1 << 12,
				LeftArrow = 1 << 13,
				UpArrow = 1 << 14,
				RightArrow = 1 << 15,
				DownArrow = 1 << 16,
				JoystickButton1 = 1 << 17,
				JoystickButton2 = 1 << 18,
				JoystickButton3 = 1 << 19,
				JoystickButton4 = 1 << 20,
				JoystickButton5 = 1 << 21,
				JoystickButton6 = 1 << 22,
				JoystickButton7 = 1 << 23,
				JoystickButton8 = 1 << 24
			};

			EventInfo();
			EventInfo(H3DF::WindowKey & cInWindowKey);

			void Set(EventInfo const & cInThat);
			EventInfo & operator = (EventInfo const & cInThat);

			void SetPoint(Command::EventInfo::Type cInType, int x, int y, UINT nInFlags);

			bool Control() const;
			bool Shift() const;
			bool Alt() const;

			bool LButton() const;
			bool MButton() const;
			bool RButton() const;

			H3DF::PixelPoint const & GetMousePixelPoint() const;
			H3DF::WindowPoint const & GetMouseWindowPoint() const;
			H3DF::WorldPoint const & GetMouseWorldPoint() const;
		};
	}
}