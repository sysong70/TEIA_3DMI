#include "stdafx.h"
#include "Connector.h"
#include "Application.h"
#include "Renderer.h"

//**************************************************************************************************

void AssignSendSignalFunc(SendSignalFunc fp)
{
	DEBUG_VALID(fp);

	// WARNING - initialize view id -1
	TheApp.GetDelivery(-1).SetSender(fp);
}



static void ReceiveSignalFromUi(const wchar_t* pContent)
{
	TheApp.ReceiveSignalFromUi(pContent);
}



bool ReceiveSignal(const wchar_t* pContent)
{
	return TheApp.ReceiveSignalFromUi(pContent);
}



void SetLanguage(int value)
{
	TheApp.SetLanguage(value);
}



double* GetCoordinate(int viewId, int x, int y)
{
	Renderer* pRenderer = TheApp.Find(viewId);
	if (pRenderer != nullptr) {
		// CHECK - UCS, WCS, ...
		OdGePoint3d world = pRenderer->GetCoordConvertor().ToEyeToWorld(x, y);
		return new double[2]{ world.x, world.y };
	}
	else {
		return new double[2]{0, 0};
	}
}
