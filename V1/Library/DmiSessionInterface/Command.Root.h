#pragma once

#include "Manager.Input.h"

namespace SESSION
{
	namespace Command
	{
		enum class Type
		{
			None		= 0x00000000,
			Line		= 0x01000000,
			Arc			= 0x01000001,
		};

		class Root
		{
		public:
			Root();
			~Root();

			// Command Type
			virtual Type Type() = 0;

			// Input Manager 관련 함수
			virtual int InputCount() = 0;
			virtual SESSION::Manager::Input::InputType * InputTypes() = 0;
		};
	}
}