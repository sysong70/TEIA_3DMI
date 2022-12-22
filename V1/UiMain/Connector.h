#pragma once

#include "Json.h"
#include "Signal.h"



typedef void (*SendSignalFunc)(const wchar_t* content);
typedef void (*AssignSendSignalFunc)(SendSignalFunc fp);



namespace Connector3d
{
	bool Initialize();

	SendSignalFunc GetSender();

	Signal::Delivery& GetInstance();

	void ReceiveSignal(const wchar_t* content);
}



namespace Connector2d
{
	bool Initialize();

	SendSignalFunc GetSender();

	Signal::Delivery& GetInstance();

	void ReceiveSignal(const wchar_t* content);
}
