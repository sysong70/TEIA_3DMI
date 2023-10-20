#pragma once

#include "Command.Root.h"

#include <Json.h>
#include <map>

namespace SESSION
{
	namespace Command
	{
		class ViewControl : public Root
		{
		public:
			ViewControl();
			~ViewControl();

			// 일단 Keyboard Input 처리 부분이 필요함.
// 			int InputCount() override;
// 			Manager::Input::InputType * InputTypes() override;
		};
	}
}