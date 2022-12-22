#pragma once

#include "Json.h"

#include <vector>

using DmiSignalCallback = void (*)(const wchar_t *);

class __declspec(dllexport) DmiHpsInterface
{
public:
	DmiHpsInterface();
	~DmiHpsInterface();

	void ExecuteSignal(Json::Object & cInObject);
	void SetSignalCallback(DmiSignalCallback lpfnSignalCallback) {
		m_lpfnSignalCallback = lpfnSignalCallback;
	}

	DmiSignalCallback GetSignalCallback() { return m_lpfnSignalCallback; }

private:
	DWORD_PTR * m_pcSignalAnalyzer;

	DmiSignalCallback m_lpfnSignalCallback = nullptr;

	// Command Analyzer로 Excute Command 전달
	bool SendExecuteCommand(Json::Object & cObject);

	void SetExecuteCommandCallBack(bool(CALLBACK * lpfnCallback)(Json::Object & cObject));

	void SendCommandCallback(Json::Object & cObject);
	void SetCommandCallback(void(CALLBACK * lpfnCallback)(DWORD_PTR *, Json::Object &), DWORD_PTR * pcTarget);

	void SetErrorMessageVector(std::vector<std::string> * pvstrErrMsgVector) {
		m_pvstrErrMsgVector = pvstrErrMsgVector;
	}

private:
	std::vector<std::string> * m_pvstrErrMsgVector = nullptr;

	bool(CALLBACK * m_lpfnCommandAnalyzerExecuteCommandCallBack)(Json::Object & cObject) = nullptr;

	DWORD_PTR * m_pcTarget = nullptr;
	void(CALLBACK * m_lpfnCommandCallback)(DWORD_PTR * pcTarget, Json::Object &) = nullptr;
};

