#include "stdafx.h"

#include "DmiSignalAnalyzer.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace Json;
using namespace std::chrono;

#define MSG_FILE_IMPORT_INFORMATION				(WM_USER + 1)

DmiSignalAnalyzer theCmdAnalyzer;

DmiSignalAnalyzer::DmiSignalAnalyzer()
{
}

//== 명령어 처리 부분 =================================================================================

// 1. 최초 명령어 처리 분기
void DmiSignalAnalyzer::ExecuteSignal(const wchar_t * pchBuffer)
{
	Json::Object cObject;
	Reader::ReadObject((wchar_t *&) pchBuffer, cObject);

	// CString strText = cObject.ToString() + L"\r\n";
	// OutputDebugString(strText);

	int nTarget = cObject.GetInteger(SKW_TARGET);

	switch((Signal::Target)nTarget)
	{
		case Signal::Target::Application: // CApp
		case Signal::Target::View:
			m_3DF_Interface.ExecuteSignal(cObject);
			break;

		default:
			assert(false);
			break;
	}
}

void DmiSignalAnalyzer::SetSignalCallback(DmiSignalCallback lpfnSignalCallback) {
	m_lpfnSignalCallback = lpfnSignalCallback;

	//m_cHpsInterface.SetSignalCallback(lpfnSignalCallback);
	m_3DF_Interface.SetSignalCallback(lpfnSignalCallback);
}
/*

void DmiSignalAnalyzer::SendCommandToMainFrame(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if(nullptr == m_lpfnMainFrameSendMessageCallback) {
		return;
	}

	m_lpfnMainFrameSendMessageCallback(nMsg, wParam, lParam);
}	*/