#pragma once

#include <Json.h>

#include "3DXSignal.h"

class DmiSignalAnalyzer;

using DmiSignalCallback = void (*)(const wchar_t *);

OPEN_3DX_SIGNAL_NAMESPACE

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

	// 삭제 예정
public:
	bool ExecuteCommand(DWORD_PTR nJsonObject);

	// Command Analyzer로 Excute Command 전달
	bool SendExecuteCommand(Json::Object & cObject);

	void SetExecuteCommandCallBack(bool(CALLBACK * lpfnCallback)(Json::Object & cObject));

protected:
	DWORD_PTR * m_pc3dxImport;

	bool(CALLBACK * m_lpfnCommandAnalyzerExecuteCommandCallBack)(Json::Object & cObject) = nullptr;
};

CLOSE_3DX_SIGNAL_NAMESPACE