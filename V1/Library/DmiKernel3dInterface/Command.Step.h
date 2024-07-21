#pragma once

#include "Object.h"

#include "Command.EventInfo.h"

namespace KERNEL
{
	namespace Command
	{
		class Step : public Object
		{
		public:
			enum class InputType
			{
				None,
				Coordinate,
				IntegerValue,
				FloatValue,
				Text,
				Count
			};

			enum class EventType
			{
				None,
				PreProcessing,
				Complete,
			};

			Step();

			InputType GetInputType() const;
			void SetInputType(InputType eInInputType);

			void SetEventInfo(const EventInfo & cInEventInfo);

			void SetMessage(CString strInMessage);
			void SetInformation(CString strInInformation);

			void SetDrawFunction(void (*pfInDrawFunction)(Step * pcInStep));

			void Draw();
		};
	}
}