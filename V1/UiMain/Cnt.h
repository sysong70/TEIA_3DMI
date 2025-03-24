#pragma once

//--------------------------------------------------------------------------------------------------

using SendSignalFunc = void (*)(const wchar_t* content);
using AssignSendSignalFunc = void (*)(SendSignalFunc fp);
using GetCoordinateFunc = double* (*)(int viewId, int x, int y);
using SetIntegerFunc = void (*)(int value);

//--------------------------------------------------------------------------------------------------

#include <Signal2d.h>

namespace Cnt2d
{
	bool Initialize();

	SendSignalFunc GetSender();

	SgnDelivery2d& GetInstance();

	SgnDelivery2d& GetInstance(int viewId);

	void ReceiveSignal(const wchar_t* content);

	double* GetCoordinate(int viewId, int x, int y);

	void SetLanguage(int value);
}

//--------------------------------------------------------------------------------------------------

#include <Signal3d.h>

namespace Cnt3d
{
	bool Initialize();

	SendSignalFunc GetSender();

	Signal::Delivery& GetInstance();

	Signal::Delivery& GetInstance(int viewId);

	void ReceiveSignal(const wchar_t* content);
}
