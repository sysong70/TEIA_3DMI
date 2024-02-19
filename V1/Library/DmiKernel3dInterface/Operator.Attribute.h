#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Operator.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Operator
	{
		class Attribute : public OperatorBase
		{
		public:
			Attribute(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			bool ShowAll();
			bool Show();
			bool Show(H3DF::SelectionItem & cSelItem);

			bool Hide();
			bool Hide(H3DF::SelectionItem & cSelItem);

			bool ShowOnly();
			bool ShowToggle();

			// void Request(Json::Object & cInObject);
			// void Change(Json::Object & cInObject);
		};
	}
}