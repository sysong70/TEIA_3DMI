#include "stdafx.h"

#include "Connector.h"
#include "Application.h"
#include "Renderer.h"

//**************************************************************************************************

void AssignSendSignalFunc(SendSignalFunc fp)
{
	DEBUG_VALID(fp);
	TheApp.SendSignalFp = fp;
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
		OdGePoint3d world = pRenderer->Coordinate.ToEyeToWorld(x, y);
		return new double[2]{ world.x, world.y };
	}
	else {
		return new double[2]{0, 0};
	}
}
