#pragma once

#include "Json.h"

class DmiSignalAnalyzer;
class Dmi3dxImport;

using DmiSignalCallback = void (*)(const wchar_t *);

class __declspec(dllexport) Dmi3dxInterface
{
public:
	Dmi3dxInterface();
	~Dmi3dxInterface();

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
	Dmi3dxImport * m_pc3dxImport;

	bool(CALLBACK * m_lpfnCommandAnalyzerExecuteCommandCallBack)(Json::Object & cObject) = nullptr;
};

