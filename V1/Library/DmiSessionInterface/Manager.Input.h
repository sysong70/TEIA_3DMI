#pragma once

#include "Manager.Base.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Input : public Base
		{
		public:
			Input();
			~Input();
		};
	}
}

extern SESSION::Manager::Input theInputManager;