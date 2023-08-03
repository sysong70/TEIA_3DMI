#include "stdafx.h"

#include "DmiKernelInterfaceDll.h"

#include "../DmiSignalAnalyzer.h"

void ExecuteCommand(const wchar_t * pchCommandText)
{
	theCmdAnalyzer.ExecuteSignal(pchCommandText);
}

void SendCommand(void (* lpfnCallback)(const wchar_t *))
{
	theCmdAnalyzer.SetSignalCallback(lpfnCallback);
}