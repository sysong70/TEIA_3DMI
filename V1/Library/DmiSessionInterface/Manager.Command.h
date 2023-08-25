#pragma once

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Command
		{
		public:
			Command();
			~Command();
		};
	}
}

extern SESSION::Manager::Command theCommandManager;