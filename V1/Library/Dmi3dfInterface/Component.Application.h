#pragma once

#include "3DF/3DF.h"

#include <Json.h>

#include <map>
#include <chrono>

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