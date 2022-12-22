#pragma once

#include "Json.h"

#include "../../Common/DmiCommandDefine.h"

namespace DmiJsonCmd
{
	// == HPS 관련 Command ==========================================================================
	bool ExecuteHpsCommand(CString strCommand, DWORD_PTR nId);
	bool ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, DWORD_PTR ptrValue);
	bool ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, int nValue1, DWORD_PTR ptrValue);
	bool ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, int nValue1, int nValue2, int nValue3);
	bool ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, int nValue1, int nValue2, int nValue3, int nValue4);
	bool ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, CString strValue);

	bool ExecuteHpsCommand(DWORD_PTR nCommandId, DWORD_PTR nId);
	bool ExecuteHpsCommand(DWORD_PTR nCommandId, DWORD_PTR nId, CString strValue);

	bool ExecuteHpsCommand(DmiHps::Command eCommandId, DWORD_PTR nId, Json::Object & cRetObject);
	bool ExecuteHpsCommand(DmiHps::Command eCommandId, DWORD_PTR nId, Json::Object & cInObject, Json::Object & cRetObject);

	void SetCommandCallback(void(CALLBACK * lpfnCallback)(DWORD_PTR *, Json::Object &), DWORD_PTR * pcTarget);

	// == C3D 관련 Command ==========================================================================
	bool ExecuteC3dCommand(CString strCommand, DWORD_PTR nId);
	bool ExecuteC3dCommand(CString strCommand, DWORD_PTR nId, CString strValue);

	// == 3DX 관련 Command ==========================================================================
	bool Execute3dxCommand(CString strCommand, DWORD_PTR nId);
	bool Execute3dxCommand(CString strCommand, DWORD_PTR nId, CString strValue);
	bool Execute3dxCommandReturnJsonMessage(CString strCommand);
};

