#include "stdafx.h"

#include "Session.SignalAnalyzer.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace Json;
using namespace std::chrono;

#define MSG_FILE_IMPORT_INFORMATION				(WM_USER + 1)

SESSION::SignalAnalyzer theSignalAnalyzer;

SESSION::SignalAnalyzer::SignalAnalyzer()
{
	// Kernel DLL을 로드한다.
	LoadSessionInterface("3DMIKernel3dInterface.DLL");
}

//== 명령어 처리 부분 =================================================================================

void SESSION::SignalAnalyzer::ExecuteSignal(const wchar_t * pchBuffer)
{
	m_pcSendSignal(pchBuffer);
 	return;
}

void SESSION::SignalAnalyzer::SetSendSignalFunc(SendSignalFunc lpfnSignalCallback)
{
	m_pcSetReceiver(lpfnSignalCallback);
}

//== DLL 관련 함수 ===================================================================================

// 1. DLL 로드
bool SESSION::SignalAnalyzer::LoadSessionInterface(const CString & strFilePath)
{
	m_hInstance = ::LoadLibrary(strFilePath);
	if (m_hInstance == nullptr) {
		m_nErrorCode = ::GetLastError();
		RETURN_FALSE;
	}

	// 호출 DLL에서 신호를 받아서 처리하는 함수를 가져온다. 
	// 상대방에서 처리하는 함수는 여기에서는 명령어를 보내는 함수가 된다.
	m_pcSendSignal = (SendSignalFunc)GetProcAddress(m_hInstance, "ExecuteCommand");

	// 호출 DLL에서 현제 DLL에 신호를 보내는 함수를 설정한다.
	// 여기에서는 명령어를 받아서 처리하는 함수가 된다.
	m_pcSetReceiver = (AssignSendSignalFunc)GetProcAddress(m_hInstance, "AssignSendSignalFunc");

	if (nullptr == m_pcSendSignal || nullptr == m_pcSetReceiver) {
		m_bIsValid = true;
		m_nErrorCode = ::GetLastError();
		RETURN_FALSE;
	}

	m_bIsValid = true;

	return true;
}