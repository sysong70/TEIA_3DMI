#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Operator.h"

#include "../Signal/Signal.h"

#include <HEventInfo.h>

namespace KERNEL
{
	class WindowKey;

	namespace Operator
	{
		class Select : public OperatorBase
		{
		public:
			Select(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			int MouseMove(HEventInfo & cInEvent);

			int LButtonDown(HEventInfo & cInEvent);
			int LButtonUp(HEventInfo & cInEvent);
		};
	}
}