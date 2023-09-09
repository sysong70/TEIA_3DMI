#pragma once

#include "Manager.Base.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Highlight : public Base
		{
		public:
			Highlight();
			~Highlight();
		};
	}
}

extern SESSION::Manager::Highlight theHighlightManager;