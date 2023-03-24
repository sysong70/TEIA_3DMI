#pragma once


#include "3DF.Signal.Interface.h"

#include "../Signal/Signal.h"

OPEN_3DF_NAMESPACE

namespace Connector
{
	SendSignalFunc GetSender();
	Signal::Delivery & GetInstance(int nViewId);
	void SetSender(void (*func)(const wchar_t *));
}

CLOSE_3DF_NAMESPACE