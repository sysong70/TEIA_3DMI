#include "stdafx.h"

#include "Interface.h"

#include "Manager.Session.h"

void SESSION::ExecuteCommand(const wchar_t * pchCommandText)
{
	theSessionManager.ExecuteSignal(pchCommandText);
}

void SESSION::AssignSendSignalFunc(void (* lpfnCallback)(const wchar_t *))
{
	theSessionManager.SetSendSignalFunc(lpfnCallback);
}