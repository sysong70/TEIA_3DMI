#pragma once

#include "Manager.Root.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Highlight : public Root
		{
		public:
			Highlight();
			~Highlight();

			Manager::Type Type() override;
		};
	}
}

extern SESSION::Manager::Highlight theHighlightManager;