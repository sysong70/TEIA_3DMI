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
		class VisualEffects : public Set
		{
		public:
			VisualEffects(const Session * pcInSession);

			void Request(Json::Object & cInObject);

			void Change(Json::Object & cInObject);
		};
	}
}