#include "stdafx.h"

#include "3DXSignal.Interface.h"

#include "Json.h"

#include "3DX.C3DImport.h"

using namespace Json;

#define CHECK_3DX if(nullptr == m_pc3dxImport) { assert(FALSE);  return false; } 

#define HOOPS_3DX_TYPE L"3dxType"

USING_3DX_SIGNAL_NAMESPACE

Interface::Interface()
{
	m_pc3dxImport = (DWORD_PTR *)new H3DX::C3dImport(this);

	if(nullptr == m_pc3dxImport) {
		MessageBox(NULL, L"Rt3dxImport init error!", L"Error", MB_OK);
	}

	assert(m_pc3dxImport);
}

Interface::~Interface()
{
	if(nullptr != m_pc3dxImport) {
		REMOVE_POINTER(m_pc3dxImport);
	}
}

void Interface::ExecuteSignal(Json::Object & cInObject)
{

}

bool Interface::ExecuteCommand(DWORD_PTR nJsonObject)
{
	Json::Object * pcObject = (Json::Object *) nJsonObject;

	CString strType = pcObject->GetString("Type");

	if(HOOPS_3DX_TYPE == strType) {
		return ((H3DX::C3dImport *)m_pc3dxImport)->ExecuteCommand(pcObject);
	}

	return false;
}

void Interface::SetExecuteCommandCallBack(bool(CALLBACK * lpfnCallback)(Json::Object & cObject))
{
	m_lpfnCommandAnalyzerExecuteCommandCallBack = lpfnCallback;
}

// Command Analyzer로 Excute Command 전달
bool Interface::SendExecuteCommand(Json::Object & cObject)
{
	if(nullptr == m_lpfnCommandAnalyzerExecuteCommandCallBack) {
		return false;
	}

	return m_lpfnCommandAnalyzerExecuteCommandCallBack(cObject);
}