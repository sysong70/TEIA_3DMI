#pragma once

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Connector
	{
		Signal::Delivery & GetInstance(int nViewId);
		void SetSender(void (*func)(const wchar_t *));
	}
}