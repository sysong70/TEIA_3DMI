#pragma once

#include "Json.h"

#include "3DX.h"

#include "3DXSignal.h"

#include <A3DSDKIncludes.h>

OPEN_3DX_NAMESPACE

class ImportBase
{
public:
	ImportBase(_3DXSignal::Interface * pc3dxInterface);
	~ImportBase();

private:
	DWORD_PTR m_nId = 0xffffff;
	_3DXSignal::Interface * m_pc3dxInterface = nullptr;

	// == 3DX 설정 관련 함수 =========================================================================
public:
	bool InitializeA3DLibrary(CString & strErrorMessage);

	bool SetLicense(CString & strErrorMessage);
	bool Reset(CString & strErrorMessage);

	//== Utility 관련 함수 ===========================================================================
protected:
	bool GetName(const A3DRootBaseWithGraphics * pcRootBase, CString & strName);

	//== 3DX Callback 관련 함수 ======================================================================
protected:
	bool SetCallbacksReport();

	static A3DInt32 CallbackReportMessage(A3DUTF8Char * chMessage);
	static A3DInt32 CallbackReportWarning(A3DUTF8Char * pcCode, A3DUTF8Char * chMessage);
	static A3DInt32 CallbackReportError(A3DUTF8Char * pcCode, A3DUTF8Char * chMessage);

	//== Error 관련 함수 =============================================================================
public:
	CString GetLastErrorMessage() { return m_strLastErrorMessage; }

protected:
	void SetLastErrorMessage(CString strErrorMessage, A3DStatus nErrorCode);
	void SetLastErrorMessage(CString strErrorMessage);

private:
	CString m_strLastErrorMessage;
	A3DStatus m_e3dxLastErrorCode;
	std::vector<CString> m_vestrMessageLog;

	//== Text 관련 함수 ==============================================================================
protected:
	bool CStringToUtf8(CString strText, A3DUTF8Char *& pchUtf8Text);
	bool Utf8ToCString(A3DUTF8Char * pchUtf8Text, CString & strText);
};

CLOSE_3DX_NAMESPACE