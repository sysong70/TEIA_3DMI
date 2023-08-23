#include "stdafx.h"

#include "KernelInterfaceDll.h"

#include "../Kernel.SignalAnalyzer.h"

void ExecuteCommand(const wchar_t * pchCommandText)
{
	theSignalAnalyzer.ExecuteSignal(pchCommandText);
}

void AssignSendSignalFunc(void (* lpfnCallback)(const wchar_t *))
{
	theSignalAnalyzer.SetSendSignalFunc(lpfnCallback);
}