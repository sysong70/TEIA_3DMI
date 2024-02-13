#pragma once

//--------------------------------------------------------------------------------------------------

using SendSignalFunc = void (*)(const wchar_t* content);
using AssignSendSignalFunc = void (*)(SendSignalFunc fp);

//--------------------------------------------------------------------------------------------------

namespace Connector3d
{
	bool Initialize();

	SendSignalFunc GetSender();

	Signal::Delivery& GetInstance();

	void ReceiveSignal(const wchar_t* content);
}

//--------------------------------------------------------------------------------------------------

namespace Connector2d
{
	bool Initialize();

	SendSignalFunc GetSender();

	Signal::Delivery& GetInstance();

	void ReceiveSignal(const wchar_t* content);
}
