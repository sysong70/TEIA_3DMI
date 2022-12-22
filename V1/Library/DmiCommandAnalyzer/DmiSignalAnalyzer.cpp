#include "stdafx.h"

#include "DmiSignalAnalyzer.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace Json;
using namespace std::chrono;

#define MSG_FILE_IMPORT_INFORMATION				(WM_USER + 1)

DmiSignalAnalyzer theCmdAnalyzer;

bool CALLBACK ExecuteCommandCallBack(Json::Object & cObject)
{
	return theCmdAnalyzer.ExecuteSignal(cObject);
}

DmiSignalAnalyzer::DmiSignalAnalyzer()
{
}

//== 명령어 처리 부분 =================================================================================

// 1. 최초 명령어 처리 분기
void DmiSignalAnalyzer::ExecuteSignal(const wchar_t * pchBuffer)
{
	Json::Object cObject;
	Reader::ReadObject((wchar_t *&) pchBuffer, cObject);

	//CString strText = cObject.ToString() + L"\r\n";
	//OutputDebugString(strText);

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

bool DmiSignalAnalyzer::ExecuteSignal(Json::Object & cObject)
{
	// #Require_convert_new_version
/*
	DmiHps::Command eType = (DmiHps::Command) Helper::GetDwordPtr(cObject, (UINT) DmiHps::Command::Type);

	switch(eType)
	{
		case DmiHps::Command::MsgHpsType:
			return ExecuteHpsCommand(cObject);
			break;
	}

	CString strType = cObject.GetString("Type");

	if(MSG_HPS_TYPE == strType) {
		return ExecuteHpsCommand(cObject);
	}
	else if(MSG_3DX_TYPE == strType) {
		return Execute3dxCommand(&cObject);
	}
	else if(MSG_C3D_TYPE == strType) {
		return ExecuteC3dCommand(&cObject);
	}
	else if(MSG_APP_TYPE == strType) {
		return ExecuteAppCommand(&cObject);
	}
*/

	return false;
}

bool DmiSignalAnalyzer::ExecuteSignal(Json::Object & cInObject, Json::Object & cRetObject)
{
	CString strType = cInObject.GetString("Type");

	// #Require_convert_new_version
/*
	if(MSG_HPS_TYPE == strType) {
		return ExecuteHpsCommand(cInObject, cRetObject);
	}
*/

	return false;
}

bool DmiSignalAnalyzer::ExecuteC3dCommand(Json::Object * pcObject)
{
	CString strCommand = pcObject->GetString("Command");

	if("DeleteModel" == strCommand) {
		DWORD_PTR nId = pcObject->GetDwordPtr("Id");
		m_C3D_Interface.DeleteModel(nId);
	}

	return m_C3D_Interface.ExecuteCommand((DWORD_PTR) pcObject);
}

// CAD 파일을 읽은 후 HPS 처리 시작
bool DmiSignalAnalyzer::Execute3dxCommand(Json::Object * pcObject)
{
	return true;
/*

	CString strCommand = pcObject->GetString("Command");

	if(L"FileImport" == strCommand) {

		DWORD_PTR nId = pcObject->GetDwordPtr("Id");

		DWORD_PTR pcModel = 0; // MbModel * type

		// 1. ID를 이용해서 기존에 생성되어 있는 Model을 가져온다.
		// 2. ID와 일치하는 Model이 없는 경우 새로운 ID를 갖는 Model을 생성한다.
		if(true == m_C3D_Interface.GetModel(nId, pcModel)) {
			pcObject->SetDwordPtr(L"Model", (DWORD_PTR) pcModel);
		}

		return m_c3dxInterface.ExecuteCommand((DWORD_PTR) pcObject);
	}

	return m_c3dxInterface.ExecuteCommand((DWORD_PTR) pcObject);*/
}

bool DmiSignalAnalyzer::ExecuteAppCommand(Json::Object * pcObject)
{
	UINT nMsg = (UINT) pcObject->GetInteger("MSG");
	SendCommandToMainFrame(nMsg, (WPARAM) pcObject, 0);
	return true;
}

void DmiSignalAnalyzer::SendCommandToMainFrame(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if(nullptr == m_lpfnMainFrameSendMessageCallback) {
		return;
	}

	m_lpfnMainFrameSendMessageCallback(nMsg, wParam, lParam);
}