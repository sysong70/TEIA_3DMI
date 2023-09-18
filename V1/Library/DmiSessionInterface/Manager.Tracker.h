#pragma once

#include "Manager.Root.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Tracker : public Root
		{
		public:
			Tracker();
			~Tracker();

			Manager::Type Type() override;
		};
	}
}

extern SESSION::Manager::Tracker theTrackerManager;