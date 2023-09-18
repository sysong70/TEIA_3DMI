#include <StdAfx.h>

#include "Manager.Session.h"

#include "Manager.Command.h"
#include "Manager.Input.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

SESSION::Manager::Session theSessionManager;

using namespace SESSION;

Manager::Session::Session()
{
	// Kernel DLL을 로드한다.
	Load3dKernelInterface("3DMIKernel3dInterface.DLL");

	theInputManager.SetSessionManager(this);

	theCommandManager.SetSessionManager(this);
}

Manager::Session::~Session()
{
	// DLL을 해제한다.
	Free3dKernelInterface();
}

Manager::Type SESSION::Manager::Session::Type()
{
	return SESSION::Manager::Type::Session;
}

//== 명령어 처리 부분 =================================================================================

void Manager::Session::ExecuteSignal(const wchar_t * pchBuffer)
{
	size_t nBufferSize = wcslen(pchBuffer) + 1; // 널 종료 문자('\0')를 포함해서 크기 계산

	// 대상 문자열에 충분한 메모리 할당
	wchar_t * pchCopyBuffer = new wchar_t[nBufferSize];
	if (nullptr == pchCopyBuffer) {
		wprintf(L"메모리 할당 실패\n");
		return;
	}

	// 문자열 복사
	wcscpy(pchCopyBuffer, pchBuffer);

	Json::Object cObject;
	// ReadObject에 buffer에 내용을 전달하고 나오면 buffer는 empty됨.
	Json::Reader::ReadObject((wchar_t *&)pchCopyBuffer, cObject);

	m_pcSendSignalTo3dKernel(pchBuffer);
}

void Manager::Session::SetSendSignalFunc(SendSignalFunc lpfnSignalCallback)
{
	m_pcSetReceiverFrom3dKernel(lpfnSignalCallback);
}

//== DLL 관련 함수 ===================================================================================

// 1. DLL 로드
bool Manager::Session::Load3dKernelInterface(const CString & strFilePath)
{
	m_hInstance = ::LoadLibrary(strFilePath);
	if (m_hInstance == nullptr) {
		m_nErrorCode = ::GetLastError();
		RETURN_FALSE;
	}

	// 호출 DLL에서 신호를 받아서 처리하는 함수를 가져온다. 
	// 상대방에서 처리하는 함수는 여기에서는 명령어를 보내는 함수가 된다.
	m_pcSendSignalTo3dKernel = (SendSignalFunc)GetProcAddress(m_hInstance, "ExecuteCommand");

	// 호출 DLL에서 현제 DLL에 신호를 보내는 함수를 설정한다.
	// 여기에서는 명령어를 받아서 처리하는 함수가 된다.
	m_pcSetReceiverFrom3dKernel = (AssignSendSignalFunc)GetProcAddress(m_hInstance, "AssignSendSignalFunc");

	if (nullptr == m_pcSendSignalTo3dKernel || nullptr == m_pcSetReceiverFrom3dKernel) {
		m_bIsValid = true;
		m_nErrorCode = ::GetLastError();
		RETURN_FALSE;
	}

	m_bIsValid = true;

	return true;
}

void Manager::Session::Free3dKernelInterface()
{
	if (nullptr != m_hInstance) {
		::FreeLibrary(m_hInstance);
	}
}