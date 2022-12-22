#pragma once

#include <vector>

#include "3DFSignal.h"

using DmiSignalCallback = void (*)(const wchar_t *);

OPEN_3DF_SIGNAL_NAMESPACE

class __declspec(dllexport) Interface
{
public:
	Interface();
	~Interface();

	void ExecuteSignal(Json::Object & cInObject);
	void SetSignalCallback(DmiSignalCallback lpfnSignalCallback) {
		m_lpfnSignalCallback = lpfnSignalCallback;
	}

	DmiSignalCallback GetSignalCallback() { return m_lpfnSignalCallback; }

private:
	DWORD_PTR * m_pcSignalManager;

	DmiSignalCallback m_lpfnSignalCallback = nullptr;

	void SetErrorMessageVector(std::vector<std::string> * pvstrErrMsgVector) {
		m_pvstrErrMsgVector = pvstrErrMsgVector;
	}

private:
	std::vector<std::string> * m_pvstrErrMsgVector = nullptr;
};

CLOSE_3DF_SIGNAL_NAMESPACE