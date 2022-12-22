#include "stdafx.h"

#include "Dmi3dxInterface.h"

#include "Json.h"

#include "../DmiCommandAnalyzer/DmiSignalAnalyzer.h"
#include "Dmi3dxImport.h"

using namespace Json;

#define CHECK_3DX if(nullptr == m_pc3dxImport) { assert(FALSE);  return false; } 

#define HOOPS_3DX_TYPE L"3dxType"

Dmi3dxInterface::Dmi3dxInterface()
{
	m_pc3dxImport = new Dmi3dxImport(this);

	if(nullptr == m_pc3dxImport) {
		MessageBox(NULL, L"Rt3dxImport init error!", L"Error", MB_OK);
	}

	assert(m_pc3dxImport);
}

Dmi3dxInterface::~Dmi3dxInterface()
{
	if(nullptr != m_pc3dxImport) {
		REMOVE_POINTER(m_pc3dxImport);
	}
}

void Dmi3dxInterface::ExecuteSignal(Json::Object & cInObject)
{

}

bool Dmi3dxInterface::ExecuteCommand(DWORD_PTR nJsonObject)
{
	Json::Object * pcObject = (Json::Object *) nJsonObject;

	CString strType = pcObject->GetString("Type");

	if(HOOPS_3DX_TYPE == strType) {
		return m_pc3dxImport->ExecuteCommand(pcObject);
	}

	return false;
}

void Dmi3dxInterface::SetExecuteCommandCallBack(bool(CALLBACK * lpfnCallback)(Json::Object & cObject))
{
	m_lpfnCommandAnalyzerExecuteCommandCallBack = lpfnCallback;
}

// Command Analyzer로 Excute Command 전달
bool Dmi3dxInterface::SendExecuteCommand(Json::Object & cObject)
{
	if(nullptr == m_lpfnCommandAnalyzerExecuteCommandCallBack) {
		return false;
	}

	return m_lpfnCommandAnalyzerExecuteCommandCallBack(cObject);
}