#pragma once

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Session;

		class Base
		{
		public:
			Base();
			~Base();

			void SetSessionManager(SESSION::Manager::Session * pcSessionManager);

		protected:
			SESSION::Manager::Session * m_pcSessionManager = nullptr;
		};
	}
}