#include "stdafx.h"

#include "Interface.h"

#include "Manager.Session.h"

// #include "../Kernel.SignalAnalyzer.h"
// #include "../Kernel.Manager.h"

void KERNEL::ExecuteCommand(const wchar_t * pchCommandText)
{
	//theSignalAnalyzer.ExecuteSignal(pchCommandText);
	theSessionManager.ExecuteSignal(pchCommandText);
}

void KERNEL::AssignSendSignalFunc(void (* lpfnCallback)(const wchar_t *))
{
	//theSignalAnalyzer.SetSendSignalFunc(lpfnCallback);
	theSessionManager.SetSendSignalFunc(lpfnCallback);
}