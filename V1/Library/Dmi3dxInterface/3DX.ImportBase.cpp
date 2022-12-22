#include "stdafx.h"

#include "3DX.ImportBase.h"

USING_3DX_NAMESPACE

// A3DSDKLoader.h 한번만 선언되어야 정상 작동함.
#include <A3DSDKLoader.h>

std::vector<CString> g_vestrMessage;
std::vector<CString> g_vestrWarning;
std::vector<CString> g_vestrError;

ImportBase::ImportBase(_3DXSignal::Interface * pc3dxInterface)
{
	m_pc3dxInterface = pc3dxInterface;
}

ImportBase::~ImportBase()
{
	A3DDllTerminate();
	A3DSDKUnloadLibrary();
}

// == 3DX 설정 관련 함수 ==============================================================================
bool ImportBase::InitializeA3DLibrary(CString & strErrorMessage)
{
	// 3DX DLL 경로 설정
	CString strDllPath = _3DX::GetExecuteDirectory() + L"3DX";

	// Dll Load
	if(A3D_FALSE == A3DSDKLoadLibrary(strDllPath)) {
		strErrorMessage = L"A3DSDKLoadLibrary Error";
		RETURN_FALSE;
	}

	// 3DX License 설정
	if(false == SetLicense(strErrorMessage)) {
		RETURN_FALSE;
	}

	// DLL Version Check
	A3DInt32 nMajorVersion, nMinorVersion;
	A3DDllGetVersion(&nMajorVersion, &nMinorVersion);
	if(A3D_DLL_MAJORVERSION != nMajorVersion || A3D_DLL_MINORVERSION != nMinorVersion)
	{
		strErrorMessage.Format(L"3DX Version Error, Required Version[%d.%d] Current Version[%d.%d]",
			A3D_DLL_MAJORVERSION, A3D_DLL_MINORVERSION,
			nMajorVersion, nMinorVersion);

		RETURN_FALSE;
	}


	A3DStatus nResult = A3DDllInitialize(A3D_DLL_MAJORVERSION, A3D_DLL_MINORVERSION);
	if(A3D_SUCCESS != nResult) {
		strErrorMessage.Format(L"3DX DLL Initialize Error: [%d]", nResult);
		RETURN_FALSE;
	}

	return true;
}

bool ImportBase::SetLicense(CString & strErrorMessage)
{
	A3DStatus nResult = A3DLicPutUnifiedLicense(HOOPS_LICENSE);
	if(A3D_SUCCESS == nResult || A3D_INITIALIZE_ALREADY_CALLED == nResult) {
		return true;
	}

	strErrorMessage.Format(L"3DX License Error: [%d]", nResult);

	return false;
}

bool ImportBase::Reset(CString & strErrorMessage)
{
	A3DDllTerminate();
	A3DSDKUnloadLibrary();

	return InitializeA3DLibrary(strErrorMessage);
}

// == Utility 함수 ==================================================================================

bool ImportBase::GetName(const A3DRootBaseWithGraphics * pcRootBase, CString & strName)
{
	A3DRootBaseData sRootBaseData;
	A3D_INITIALIZE_DATA(A3DRootBaseData, sRootBaseData);

	A3DStatus eStstus = A3DRootBaseGet(pcRootBase, &sRootBaseData);
	if(A3D_SUCCESS != eStstus) {
		m_e3dxLastErrorCode = eStstus;
		SetLastErrorMessage(L"Get Name - Root Base Get Error", eStstus);
		return false;
	}

	bool bStatus = false;
	if(nullptr != sRootBaseData.m_pcName) {
		bStatus = Utf8ToCString(sRootBaseData.m_pcName, strName);
	}

	//ASSERT( sRootBaseData.m_uiPersistentId ==0);
	//ASSERT( sRootBaseData.m_uiNonPersistentId == 0);

	A3DRootBaseGet(nullptr, &sRootBaseData);

	return bStatus;
}

//+ ===== 3DX Callback 함수 ==============================================

bool ImportBase::SetCallbacksReport()
{
	g_vestrMessage.clear();
	g_vestrWarning.clear();
	g_vestrError.clear();

	A3DStatus nStatus = A3DDllSetCallbacksReport(CallbackReportMessage, CallbackReportWarning, CallbackReportError);
	if(A3D_SUCCESS == nStatus) {
		return true;
	}

	return false;
}

A3DInt32 ImportBase::CallbackReportMessage(A3DUTF8Char * chMessage)
{
	CString strMsg;
	_3DX::CharToCString(chMessage, strMsg, CP_UTF8);
	g_vestrMessage.push_back(strMsg);
	/*
		int pos = strMsg.Find(_T("Version/Release"));
		if ( pos !=-1)
		{
			pos = strMsg.Find(_T(":"));
			if ( pos != -1)
			{
				m_strFileVersion = strMsg.Right( strMsg.GetLength() - pos -1);
				m_strFileVersion.Trim();
			}
		}

		TRACE(_T("%s\n\r"), strMsg);

	*/
	return 0;
}

A3DInt32 ImportBase::CallbackReportWarning(A3DUTF8Char * pcCode, A3DUTF8Char * chMessage)
{
	CString strMsg;
	_3DX::CharToCString(chMessage, strMsg, CP_UTF8);
	g_vestrWarning.push_back(strMsg);

	return 0;
}

A3DInt32 ImportBase::CallbackReportError(A3DUTF8Char * pcCode, A3DUTF8Char * chMessage)
{
	CString strMsg;
	_3DX::CharToCString(chMessage, strMsg, CP_UTF8);
	g_vestrError.push_back(strMsg);

	return 0;
}

// == Error 처리 관련 함수 ===========================================================================

void ImportBase::SetLastErrorMessage(CString strErrorMessage, A3DStatus eErrorCode)
{
	m_strLastErrorMessage.Format(L"%s [%d]", strErrorMessage, eErrorCode);
	m_e3dxLastErrorCode = eErrorCode;

	m_vestrMessageLog.push_back(m_strLastErrorMessage);
}

void ImportBase::SetLastErrorMessage(CString strErrorMessage)
{
	SetLastErrorMessage(strErrorMessage, m_e3dxLastErrorCode);
}

// == Text 관련 함수 ================================================================================

bool ImportBase::CStringToUtf8(CString strText, A3DUTF8Char *& pchUtf8Text)
{
	if(true == strText.IsEmpty()) {
		return false;
	}

	int nTextLength = strText.GetLength() * sizeof(WCHAR);
	pchUtf8Text = new A3DUTF8Char[nTextLength];

	// to UTF8
	memset(pchUtf8Text, 0, nTextLength);
	A3DStatus eStatus = A3DMiscUnicodeToUTF8((A3DUTF8Char *) (LPCTSTR) strText, pchUtf8Text);
	if(A3D_SUCCESS != eStatus) {
		SetLastErrorMessage(L"CString To Utf8 Error", eStatus);
		return false;
	}

	return true;
}

bool ImportBase::Utf8ToCString(A3DUTF8Char * pchUtf8Text, CString & strText)
{
	return _3DX::CharToCString(pchUtf8Text, strText, CP_UTF8);
}