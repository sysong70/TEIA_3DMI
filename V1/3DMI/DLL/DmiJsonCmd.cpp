#include "pch.h"
#include "DmiJsonCmd.h"

#include "../DmiApp.h"

#include <Json.h>

#include "../../Common/DmiCommandDefine.h"

// == HPS 관련 Command ==============================================================================
bool DmiJsonCmd::ExecuteHpsCommand(CString strCommand, DWORD_PTR nId)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, DWORD_PTR ptrValue)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);
	cObject.SetDwordPtr("Value", ptrValue);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, int nValue1, DWORD_PTR ptrValue)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);
	cObject.SetDwordPtr("Value1", nValue1);
	cObject.SetDwordPtr("Value2", ptrValue);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, int nValue1, int nValue2, int nValue3)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);
	cObject.SetDwordPtr("Value1", nValue1);
	cObject.SetDwordPtr("Value2", nValue2);
	cObject.SetDwordPtr("Value3", nValue3);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, int nValue1, int nValue2, int nValue3, int nValue4)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);
	cObject.SetDwordPtr("Value1", nValue1);
	cObject.SetDwordPtr("Value2", nValue2);
	cObject.SetDwordPtr("Value3", nValue3);
	cObject.SetDwordPtr("Value4", nValue4);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::ExecuteHpsCommand(CString strCommand, DWORD_PTR nId, CString strValue)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);
	cObject.SetString("Value", strValue);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::ExecuteHpsCommand(DWORD_PTR nCommandId, DWORD_PTR nId)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetDwordPtr("CommandId", nCommandId);
	cObject.SetDwordPtr("Id", nId);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::ExecuteHpsCommand(DWORD_PTR nCommandId, DWORD_PTR nId, CString strValue)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetDwordPtr("CommandId", nCommandId);
	cObject.SetDwordPtr("Id", nId);
	cObject.SetString("Value", strValue);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::ExecuteHpsCommand(DmiHps::Command eCommandId, DWORD_PTR nId, Json::Object & cRetObject)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_HPS_TYPE);
	cObject.SetDwordPtr("CommandId", (DWORD_PTR) eCommandId);
	cObject.SetDwordPtr("Id", nId);

	return theApp.ExecuteCommand(cObject, cRetObject);
}

bool DmiJsonCmd::ExecuteHpsCommand(DmiHps::Command eCommandId, DWORD_PTR nId, Json::Object & cInObject, Json::Object & cRetObject)
{
	cInObject.SetString("Type", MSG_HPS_TYPE);
	cInObject.SetDwordPtr("CommandId", (DWORD_PTR) eCommandId);
	cInObject.SetDwordPtr("Id", nId);

	return theApp.ExecuteCommand(cInObject, cRetObject);
}

void DmiJsonCmd::SetCommandCallback(void(CALLBACK * lpfnCallback)(DWORD_PTR *, Json::Object &), DWORD_PTR * pcTarget)
{
	theApp.SetMessageCallback(lpfnCallback, pcTarget);
}

// == C3D 관련 Command ==============================================================================
bool DmiJsonCmd::ExecuteC3dCommand(CString strCommand, DWORD_PTR nId)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_C3D_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::ExecuteC3dCommand(CString strCommand, DWORD_PTR nId, CString strValue)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_C3D_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);
	cObject.SetString("Value", strValue);

	return theApp.ExecuteCommand(cObject);
}

// == 3DX 관련 Command ==============================================================================
bool DmiJsonCmd::Execute3dxCommand(CString strCommand, DWORD_PTR nId)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_3DX_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::Execute3dxCommand(CString strCommand, DWORD_PTR nId, CString strValue)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_3DX_TYPE);
	cObject.SetString("Command", strCommand);
	cObject.SetDwordPtr("Id", nId);
	cObject.SetString("Value", strValue);

	return theApp.ExecuteCommand(cObject);
}

bool DmiJsonCmd::Execute3dxCommandReturnJsonMessage(CString strCommand)
{
	Json::Object cObject;

	cObject.SetString("Type", MSG_3DX_TYPE);
	cObject.SetString("Command", strCommand);

	return theApp.ExecuteCommand(cObject);
}

