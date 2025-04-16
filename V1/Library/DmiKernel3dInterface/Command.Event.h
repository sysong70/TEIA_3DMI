#pragma once

#include "Object.h"

#include <3DF/Point.h>
#include <3DF/Window.h>
#include <3DF/Operator.Event.h>

namespace KERNEL
{
	namespace Command
	{
		class Event : public Object
		{
		public:
			Event();
			Event(H3DF::WindowKey cInWindowKey);
			Event(Event const & cInThat);

			void Set(Event const & cInThat);
			Event & operator = (Event const & cInThat);

			H3DF::Operator::Event & OperatorEvent();

			void SetPoint(H3DF::Operator::Event::Type cInType, int x, int y, UINT nInFlags);
			void SetMouseWheelDelta(int nInDelta);

			bool Control() const;
			bool Shift() const;
			bool Alt() const;

			bool LButton() const;
			bool MButton() const;
			bool RButton() const;

			H3DF::PixelPoint const & GetMousePixelPoint() const;
			H3DF::WindowPoint const & GetMouseWindowPoint() const;
			H3DF::WorldPoint const & GetMouseWorldPoint() const;

			void SetEventType(H3DF::Operator::Event::Type cInType);
			H3DF::Operator::Event::Type GetEventType() const;
		};

		using EventArray = std::vector<Event>;
	}
}