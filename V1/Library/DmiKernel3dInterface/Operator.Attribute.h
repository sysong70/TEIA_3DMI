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
			Attribute(const DocView * pcInDocView);

			bool ShowAll();
			bool Show(H3DF::Key & cKey);
			bool Show(H3DF::SelectionItem & cSelItem);

			bool NoShow(H3DF::Key & cKey);
			bool NoShow(H3DF::SelectionItem & cSelItem);

			bool ShowOnly();
			bool ShowOnly(H3DF::Key & cKey);
			bool ShowOnly(H3DF::SelectionItem & cSelItem);

			bool ShowToggle();

			// void Request(Json::Object & cInObject);
			// void Change(Json::Object & cInObject);
		};
	}
}