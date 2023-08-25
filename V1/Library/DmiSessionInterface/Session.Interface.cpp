#include "stdafx.h"

#include "Session.Interface.h"

#include "Manager.Session.h"

void ExecuteCommand(const wchar_t * pchCommandText)
{
	theSessionManager.ExecuteSignal(pchCommandText);
}

void AssignSendSignalFunc(void (* lpfnCallback)(const wchar_t *))
{
	theSessionManager.SetSendSignalFunc(lpfnCallback);
}