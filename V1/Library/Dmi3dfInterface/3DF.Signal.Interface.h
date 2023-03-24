#pragma once

#include <vector>

#include "3DF.Signal.h"

OPEN_3DF_NAMESPACE

using SendSignalFunc = void (*)(const wchar_t * content);

class __declspec(dllexport) Interface
{
public:
	Interface();
	~Interface();

	void ExecuteSignal(Json::Object & cInObject);
	void SetSignalCallback(SendSignalFunc lpfnSendSignalFunc);

	SendSignalFunc GetSendSignalFunc() { return m_lpfnSendSignalFunc; }

private:
	DWORD_PTR * m_pcSignalManager;

	SendSignalFunc m_lpfnSendSignalFunc = nullptr;

	void SetErrorMessageVector(std::vector<std::string> * pvstrErrMsgVector) {
		m_pvstrErrMsgVector = pvstrErrMsgVector;
	}

private:
	std::vector<std::string> * m_pvstrErrMsgVector = nullptr;
};

CLOSE_3DF_NAMESPACE