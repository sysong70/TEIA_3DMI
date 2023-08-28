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

}

//== 명령어 처리 부분 =================================================================================

void Manager::Session::ExecuteSignal(const wchar_t * pchBuffer)
{
// 	Json::Object cObject;
// 	Json::Reader::ReadObject((wchar_t *&)pchBuffer, cObject);
// 
// 	int nViewId = cObject.GetInteger(SKW_VIEWID);
// 	int nTarget = cObject.GetInteger(SKW_TARGET);

	m_pcSendSignalTo3dKernel(pchBuffer);
	return;
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