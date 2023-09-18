#pragma once

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		enum class Type : uint32_t
		{
			None				= 0x00000000,
			Session				= 0x04000000,
			Command				= 0x04000001,
			Input				= 0x04000002,
			Selection			= 0x04000003,
			Highlight			= 0x04000004,
			Tracker				= 0x04000005,
			Tree				= 0x04000006,
			ObjectSnap			= 0x04000007,

		};

		class Session;

		class Root
		{
		public:
			Root();
			~Root();

			virtual Type Type() = 0;

			void SetSessionManager(SESSION::Manager::Session * pcSessionManager);

		protected:
			SESSION::Manager::Session * m_pcSessionManager = nullptr;
		};
	}
}