#pragma once

#include "../Dmi3dfInterface/3DF.Signal.Interface.h"
#include "../Dmi3dxInterface/3DXSignal.Interface.h"
#include "../DmiC3dInterface/DmiC3dInterface.h"

#include "Json.h"

#include <chrono>

using DmiSignalCallback = void (*)(const wchar_t *);

class DmiSignalAnalyzer
{
public:
	DmiSignalAnalyzer();

	void ExecuteSignal(const wchar_t * pchBuffer);
	void SetSignalCallback(DmiSignalCallback lpfnSignalCallback);

	DmiSignalCallback GetSignalCallback() { return m_lpfnSignalCallback; }

private:
	_3DF::Interface m_3DF_Interface;

	//Dmi3dxInterface m_c3dxInterface;

	DmiC3dInterface m_C3D_Interface;

	DmiSignalCallback m_lpfnSignalCallback = nullptr;


	// 삭제 예정
public:
	bool ExecuteSignal(Json::Object & cObject);
	bool ExecuteSignal(Json::Object & cInObject, Json::Object & cRetObject);

	void SendCommandToMainFrame(UINT nMsg, WPARAM wParam, LPARAM lParam);
	
	void SetMainFrameSendMessageCallback(LRESULT(CALLBACK * lpfnCallback)(UINT, WPARAM, LPARAM)) {
		m_lpfnMainFrameSendMessageCallback = lpfnCallback;
	}

protected:
	bool ExecuteC3dCommand(Json::Object * pcObject);

	bool Execute3dxCommand(Json::Object * pcObject);

	bool ExecuteAppCommand(Json::Object * pcObject);

	// 삭제 예정
	LRESULT(CALLBACK * m_lpfnMainFrameSendMessageCallback)(UINT, WPARAM, LPARAM) = nullptr;

	std::vector<std::string> m_vstrHpsErrMsgVector;
};

extern DmiSignalAnalyzer theCmdAnalyzer;