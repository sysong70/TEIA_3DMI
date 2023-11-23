#include "stdafx.h"

#include "3DF.Application.h"

#include <Common_Define.h>
#include <LogManager.h>

#include "../Signal/Signal.h"

// #include "3DF.Signal.Manager.h"
// #include "3DF.Signal.Interface.h"

#include "3DF/3DF.Utility.h"

// ----- HOOPS Header -----
#include <hoops_license.h>

#include <HDB.h>
#include <HErrorManager.h>
#include <hpserror.h>

//:Ken
#include "3DF/Facility.AppOptions.h"

using namespace H3DF;

namespace H3DF
{
	class ApplicationPrivate : public PrivateImpl
	{
	public:
		HDB * m_pcHoopsDB = nullptr;
		static void ErrorCallback(HErrorNode * pcNode, void * pcUserData);
		static CStringA ErrorCategoryString(int nId);
		static CStringA ErrorSpecificString(int nId);
	};
}

void H3DF::ApplicationPrivate::ErrorCallback(HErrorNode * pcNode, void * pcUserData)
{
	CStringA strMessage, strText;
	

	switch (pcNode->m_severity) {
		case 0:
			strMessage = "[Info] ";
			break;

		case 1:
			strMessage = "[Warning] ";
			break;

		case 2:
			strMessage = L"[Error] ";
			break;

		case 3:
			strMessage = L"[Fatal Error] ";
			break;
	}

	strText.Format("Category: %s, ", ErrorCategoryString(pcNode->m_category));
	strMessage += strText;

	strText.Format("Specific: %s", ErrorSpecificString(pcNode->m_specific));
	strMessage += strText;

	LogManager::Log(LOGMANAGER_3DF_ERROR_LOG_ID, strMessage);

	LogManager::IncreaseTabIndex(LOGMANAGER_3DF_ERROR_LOG_ID);
	for (int nIndex = 0; nIndex < pcNode->m_messagelength; nIndex++) {
		strMessage.Format("ErrMsg: %s", pcNode->m_message[nIndex]);
		LogManager::Log(LOGMANAGER_3DF_ERROR_LOG_ID, strMessage);
	}

	for (int nIndex = 0; nIndex < pcNode->m_stacklength; nIndex++) {
		strMessage.Format("Stack: %s", pcNode->m_stack[nIndex]);
		LogManager::Log(LOGMANAGER_3DF_ERROR_LOG_ID, strMessage);
	}

	LogManager::DecreaseTabIndex(LOGMANAGER_3DF_ERROR_LOG_ID);
}

CStringA H3DF::ApplicationPrivate::ErrorCategoryString(int nId)
{
	CStringA strMessage;
	switch (nId)
	{
		case HEC_UNDEFINED:
			return strMessage = "Undefined";
			break;

		case HEC_DEBUG:
			return strMessage = "Debug";
			break;

		case HEC_INTERNAL_ERROR:
			return strMessage = "Internal Error";
			break;

		case HEC_MEMORY:
			return strMessage = "Memory";
			break;

		case HEC_CALLBACK:
			return strMessage = "Callback";
			break;

		case HEC_CAMERA:
			return strMessage = "Camera";
			break;

		case HEC_COLOR:
			return strMessage = "Color";
			break;

		case HEC_COLOR_MAP:
			return strMessage = "Color Map";
			break;

		case HEC_DRIVER:
			return strMessage = "Driver";
			break;

		case HEC_HANDEDNESS:
			return strMessage = "Handedness";
			break;

		case HEC_HEURISTIC:
			return strMessage = "Heuristic";
			break;

		case HEC_HOOPS_SYSTEM:
			return strMessage = "Hoops System";
			break;

		case HEC_INVALID_KEY:
			return strMessage = "Invalid key";
			break;

		case HEC_SYNTAX:
			return strMessage = "Syntax";
			break;

		case HEC_INCLUDE:
			return strMessage = "Include";
			break;

		default:
			strMessage.Format("%d", nId);
			break;
	}
	
	return strMessage;
}

CStringA H3DF::ApplicationPrivate::ErrorSpecificString(int nId)
{
	CStringA strMessage;
	switch (nId)
	{
		case HES_UNDEFINED:
			return strMessage = "Undefined";
			break;

		case HES_DEBUG:
			return strMessage = "Debug";
			break;

		case HES_PROCEDURAL_ERROR:
			return strMessage = "Procedural Error";
			break;

		case HES_DATA_ERROR:
			return strMessage = "Data Error";
			break;
		
		case HES_OUT_OF_MEMORY:
			return strMessage = "Out of Memory";
			break;

		case HES_BUFFER_OVERFLOW:
			return strMessage = "Buffer Overflow";
			break;

		case HES_ALLOCATE_MEMORY_FAILED:
			return strMessage = "Allocate Memory Failed";
			break;

		case HES_MEMORY_USAGE:
			return strMessage = "Memory Usage";
			break;

		case HES_SYSTEM_REJECTED_FREED_MEMORY:
			return strMessage = "System Rejected Freed Memory";
			break;

		case HES_MEMORY_PROFILE_NOT_COMPILED:
			return strMessage = "Memory Profile Not Compiled";
			break;

		case HES_ABORT_FUNCTION:
			return strMessage = "Abort Function";
			break;

		case HES_ABORT_REQUESTED_BY_APPLICATION:
			return strMessage = "Abort Requested By Application";
			break;

		case HES_INVALID_KEY:
			return strMessage = "Invalid key";
			break;

		case HES_PARSE_STRING:
			return strMessage = "Parse string";
			break;

		default:
			strMessage.Format("%d", nId);
			break;
	}

	return strMessage;
}

//== CWinApp에서 전달되는 메시지 처리 ==================================================================

// 1. Application이 실행될때 최초 처리 CWinApp::InitInstance에서 메시지 전달 받음.
void H3DF::Application::InitInstance()
{
	ApplicationPrivate * pcImpl = new ApplicationPrivate();
	if (nullptr == pcImpl) {
		assert(false);
	}

	m_pcImpl = pcImpl;

	//----- Construct에서 처리 하는 부분 -----
	// HOOPS License 처리
	HC_Define_System_Options("license = `" HOOPS_LICENSE "`");

	HDB::EnableErrorManager();

	// Error 및 Warning 관련 설정
#if defined( _DEBUG )
	//HC_Define_System_Options("fatal errors, errors, warnings, info, no message limit");
	HErrorManager::AllowAllErrors();
	HErrorManager::SetErrorCallback(H3DF::ApplicationPrivate::ErrorCallback, this);
	LogManager::SetCreateFile(LOGMANAGER_3DF_ERROR_LOG_ID, true);
	LogManager::SetFilePathName(LOGMANAGER_3DF_ERROR_LOG_ID, LogManager::GetExecuteDirectory() + L"Log\\3DF_Error.txt");
#else
	HC_Define_System_Options("errors, info, no message limit");
	//HC_Define_System_Options("no warnings, no info, no errors, no fatal errors, no message limit");
#endif

	//----- InitInstance에서 처리하는 부분 -----
	HANDLE heaps[1025];
	DWORD nheaps = GetProcessHeaps(1024, heaps);

	for(DWORD i = 0; i < nheaps; i++) {
		ULONG  HeapFragValue = 2;
		HeapSetInformation(heaps[i], HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue));
	}

	pcImpl->m_pcHoopsDB = new HDB;
	pcImpl->m_pcHoopsDB->Init();

	// set the font directory
	TCHAR fontDirectory[MAX_PATH + 32];
	::GetWindowsDirectory(fontDirectory, MAX_PATH);
//	
	_tcscat(fontDirectory, _T("\\Fonts"));

	CString strFontDirectory = Utility::GetExecuteDirectory() + L"Fonts";

	CStringA strBuffer;
	strBuffer.Format("font directory = (%s, .)", Utility::ToChar(strFontDirectory));
	strBuffer.Format("%s, multi-threading=%s", strBuffer, "full");

/*
	char buf[4096];
 	sprintf(buf, "font directory = (%s, .)", Utility::ToChar(strFontDirectory));
	sprintf(buf, "%s, multi-threading=%s", buf, "full");
*/

	HC_Define_System_Options(strBuffer);
}

// 2. CWinApp::OnExitInstance() 처리
void H3DF::Application::ExitInstance()
{
	ApplicationPrivate * pcImpl = static_cast<ApplicationPrivate *>(m_pcImpl);

	if (nullptr != pcImpl->m_pcHoopsDB) {
		delete pcImpl->m_pcHoopsDB;
		pcImpl->m_pcHoopsDB = nullptr;
	}

	HErrorManager::Cleanup();
}