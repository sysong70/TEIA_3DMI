#pragma once

#include "Manager.Root.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Tree : public Root
		{
		public:
			Tree();
			~Tree();

			Manager::Type Type() override;
		};
	}
}

extern SESSION::Manager::Tree theTreeManager;