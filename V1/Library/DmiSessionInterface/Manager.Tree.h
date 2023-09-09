#pragma once

#include "Manager.Base.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Tree : public Base
		{
		public:
			Tree();
			~Tree();
		};
	}
}

extern SESSION::Manager::Tree theTreeManager;