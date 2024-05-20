#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Command.h"
#include "Command.Set.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Command
	{
		class Attribute : public Set
		{
		public:
			Attribute(const Session * pcInSession);

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