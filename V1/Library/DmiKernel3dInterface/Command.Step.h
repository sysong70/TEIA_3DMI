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

			void SetEventInfo(std::vector<Command::EventInfo> & vcEventInfos, Command::EventInfo & cInEvent);

			Command::EventInfoArray & GetSavedEvents();
			Command::EventInfo & GetEvent();

			void SetMessage(CString strInMessage);
			void SetInformation(CString strInInformation);

			void SetDrawFunction(void (*pfInDrawFunction)(Step * pcInStep, H3DF::SegmentKey cInSegment));

			void Draw(H3DF::SegmentKey cInSegment);
		};
	}
}