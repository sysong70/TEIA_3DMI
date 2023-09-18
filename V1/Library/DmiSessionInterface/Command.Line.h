#pragma once

#include "Command.Root.h"

#include <Json.h>
#include <map>

namespace SESSION
{
	namespace Command
	{
		class Line : public Root
		{
		public:
			Line();
			~Line();

			int InputCount() override;

			Manager::Input::InputType * InputTypes() override;
		};
	}
}