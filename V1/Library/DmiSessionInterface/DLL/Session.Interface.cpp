#include "stdafx.h"

#include "Session.Interface.h"

#include "../Session.SignalAnalyzer.h"
#include "../Session.Manager.h"


void ExecuteCommand(const wchar_t * pchCommandText)
{
	theSessionManager.ExecuteSignal(pchCommandText);

	//theSignalAnalyzer.ExecuteSignal(pchCommandText);
}

void AssignSendSignalFunc(void (* lpfnCallback)(const wchar_t *))
{
	theSessionManager.SetSendSignalFunc(lpfnCallback);

	//theSignalAnalyzer.SetSendSignalFunc(lpfnCallback);
}