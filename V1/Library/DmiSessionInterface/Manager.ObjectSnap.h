#pragma once

#include "Manager.Base.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class ObjectSnap : public Base
		{
		public:
			ObjectSnap();
			~ObjectSnap();
		};
	}
}

extern SESSION::Manager::ObjectSnap theObjectSnapManager;