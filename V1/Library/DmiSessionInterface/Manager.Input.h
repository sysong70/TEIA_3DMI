#pragma once

#include "Manager.Root.h"

#include "Json.h"

#include <map>

namespace SESSION
{
	namespace Manager
	{
		class Input : public Root
		{
		public:
			enum class InputType : uint32_t
			{
				None				= 0x00000000,
				Cordinate			= 0x01000000,
			};
			
			Input();
			~Input();

			Manager::Type Type() override;
		};
	}
}

extern SESSION::Manager::Input theInputManager;