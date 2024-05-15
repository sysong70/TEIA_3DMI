#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Command.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Command
	{
		class Attribute : public CommandBase
		{
		public:
			Attribute(const DocView * pcInDocView);

			bool ShowAll();
			bool Show(H3DF::Component * pcInComponent);
			bool Show(H3DF::SelectionItem & cSelItem);

			bool NoShow(H3DF::Component * pcInComponent);
			bool NoShow(H3DF::SelectionItem & cSelItem);

			bool HideOnly();
			bool ShowOnly();

			bool ShowToggle();

			bool ShowReset();

			// void Request(Json::Object & cInObject);
			// void Change(Json::Object & cInObject);
		};
	}
}