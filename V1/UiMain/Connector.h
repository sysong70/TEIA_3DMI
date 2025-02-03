#pragma once

//--------------------------------------------------------------------------------------------------

using SendSignalFunc = void (*)(const wchar_t* content);
using AssignSendSignalFunc = void (*)(SendSignalFunc fp);
using GetCoordinateFunc = double* (*)(int viewId, int x, int y);
using SetIntegerFunc = void (*)(int value);

//--------------------------------------------------------------------------------------------------

namespace Connector3d
{
	bool Initialize();

	SendSignalFunc GetSender();

	Signal::Delivery& GetInstance();

	Signal::Delivery& GetInstance(int viewId);

	void ReceiveSignal(const wchar_t* content);
}

//--------------------------------------------------------------------------------------------------

namespace Connector2d
{
	bool Initialize();

	SendSignalFunc GetSender();

	Signal::Delivery& GetInstance();

	Signal::Delivery& GetInstance(int viewId);

	void ReceiveSignal(const wchar_t* content);

	double* GetCoordinate(int viewId, int x, int y);

	void SetLanguage(int value);
}
