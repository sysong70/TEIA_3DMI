#pragma once

#include "Manager.Base.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Command : public Base
		{
		public:
			Command();
			~Command();

			void ExecuteSignal(const wchar_t * pchBuffer);
		};
	}
}

extern SESSION::Manager::Command theCommandManager;