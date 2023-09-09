#include "stdafx.h"

#include "Interface.h"

#include "Interface.Session.h"

void KERNEL::ExecuteCommand(const wchar_t * pchCommandText)
{
	theSessionInterface.ExecuteSignal(pchCommandText);
}

void KERNEL::AssignSendSignalFunc(void (* lpfnCallback)(const wchar_t *))
{
	theSessionInterface.SetSendSignalFunc(lpfnCallback);
}