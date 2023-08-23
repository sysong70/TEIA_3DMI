#include "stdafx.h"

#include "Session.Interface.h"

#include "../Kernel.SignalAnalyzer.h"
#include "../Kernel.Manager.h"


void ExecuteCommand(const wchar_t * pchCommandText)
{
	theSignalAnalyzer.ExecuteSignal(pchCommandText);
}

void AssignSendSignalFunc(void (* lpfnCallback)(const wchar_t *))
{
	theSignalAnalyzer.SetSendSignalFunc(lpfnCallback);
}