#include "stdafx.h"
#include "Connector.h"
#include "Application.h"

//--------------------------------------------------------------------------------------------------

bool Initialize()
{
	return TheApp.Initialize();
}



bool Terminate()
{
	return TheApp.Terminate();
}



void AssignSendSignalFunc(SendSignalFunc fp)
{
	DEBUG_VALID(fp);
	TheApp.SendSignalToUi = fp;
}



bool ReceiveSignal(const wchar_t* content)
{
	return TheApp.ReceiveSignalFromUi(content);
}
