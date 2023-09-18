#pragma once

#include "Manager.Root.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class ObjectSnap : public Root
		{
		public:
			ObjectSnap();
			~ObjectSnap();

			Manager::Type Type() override;
		};
	}
}

extern SESSION::Manager::ObjectSnap theObjectSnapManager;