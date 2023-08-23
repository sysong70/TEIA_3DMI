#pragma once

#include <Json.h>

#include <map>
#include <chrono>

#include "3DF.Component.h"

#include "../Signal/Signal.h"

class HDB;

namespace H3DF
{
	namespace Component
	{
		class API_3DF Application
		{
		public:
			void InitInstance();
			void ExitInstance();

		protected:
			HDB * m_pcHoopsDB = nullptr;
		};
	};
};