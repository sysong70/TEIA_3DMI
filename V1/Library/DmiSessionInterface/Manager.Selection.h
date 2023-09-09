#pragma once

#include "Manager.Base.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Selection : public Base
		{
		public:
			Selection();
			~Selection();
		};
	}
}

extern SESSION::Manager::Selection theSelectionManager;