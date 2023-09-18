#pragma once

#include "Manager.Root.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Selection : public Root
		{
		public:
			Selection();
			~Selection();

			Manager::Type Type() override;
		};
	}
}

extern SESSION::Manager::Selection theSelectionManager;