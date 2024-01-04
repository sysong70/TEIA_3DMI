#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Operator.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Operator
	{
		class Visibility : public OperatorBase
		{
		public:
			Visibility(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			bool ShowAll();
			bool Hide();
			bool ShowOnly();
			bool Toggle();

			// void Request(Json::Object & cInObject);
			// void Change(Json::Object & cInObject);
		};
	}
}