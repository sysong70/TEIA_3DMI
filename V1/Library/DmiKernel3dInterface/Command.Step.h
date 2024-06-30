#pragma once

#include "Object.h"

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
				Text
			};

			Step();

			InputType GetInputType() const;
			void SetInputType(InputType eInInputType);

			void SetMessage(CString strInMessage);
			void SetInformation(CString strInInformation);

			void SetDrawFunction(void (*pfInDrawFunction)(Step * pcInStep));

			void Draw();
		};
	}
}