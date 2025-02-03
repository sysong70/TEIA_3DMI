#pragma once

#include "Command.Base.h"

//--------------------------------------------------------------------------------------------------

namespace Command
{
	class VisualEffects3d : public Base
	{
	public:

		VisualEffects3d() {}

	public:

		bool ReceiveSignal(Json::Object* pData) override;

		void Run(Window::View* pView) override;
	};
}
