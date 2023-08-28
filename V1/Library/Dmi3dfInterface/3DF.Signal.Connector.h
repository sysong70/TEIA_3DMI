#pragma once

#include "../Signal/Signal.h"

namespace H3DF
{
	namespace Connector
	{
		//SendSignalFunc GetSender();
		Signal::Delivery & GetInstance(int nViewId);
		void SetSender(void (*func)(const wchar_t *));
	}
}
