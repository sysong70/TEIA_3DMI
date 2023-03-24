#include "stdafx.h"

#include "3DF.Signal.ApplicationManager.h"

#include <Common_Define.h>

#include "../Signal/Signal.h"

#include "3DF.Signal.Manager.h"
#include "3DF.Signal.Interface.h"

// ----- HOOPS Header -----
#include <hoops_license.h>

#include <HDB.h>

USING_3DF_NAMESPACE

ApplicationManager theApplicationManager;

//== 전달 받은 명령어 분기 =============================================================================

void ApplicationManager::ExecuteSignal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);

	//Wrapper().m_pcHoopsDB = nullptr;

	switch((Signal::Application::Action) nAction)
	{
		case Signal::Application::Action::OnInitInstance:
			InitInstance();
			break;

		case Signal::Application::Action::OnExitInstance:
			ExitInstance();
			break;

		default:
			assert(false);
			break;
	}
}

//== CWinApp에서 전달되는 메시지 처리 ==================================================================

// 1. Application이 실행될때 최초 처리 CWinApp::InitInstance에서 메시지 전달 받음.
void ApplicationManager::InitInstance()
{
	//----- Construct에서 처리 하는 부분 -----
	// HOOPS License 처리
	HC_Define_System_Options("license = `" HOOPS_LICENSE "`");

	// Error 및 Warning 관련 설정
#if defined( _DEBUG )
	HC_Define_System_Options("errors, info, no message limit");
#else
	HC_Define_System_Options("errors, info, no message limit");
	//HC_Define_System_Options("no warnings, no info, no errors, no fatal errors, no message limit");
#endif

	HDB::EnableErrorManager();

	//----- InitInstance에서 처리하는 부분 -----
	HANDLE heaps[1025];
	DWORD nheaps = GetProcessHeaps(1024, heaps);

	for(DWORD i = 0; i < nheaps; i++) {
		ULONG  HeapFragValue = 2;
		HeapSetInformation(heaps[i], HeapCompatibilityInformation, &HeapFragValue, sizeof(HeapFragValue));
	}

	Wrapper().m_pcHoopsDB = new HDB;
	Wrapper().m_pcHoopsDB->Init();

	// set the font directory
	TCHAR fontDirectory[MAX_PATH + 32];
	::GetWindowsDirectory(fontDirectory, MAX_PATH);
//	
	_tcscat(fontDirectory, _T("\\Fonts"));

	CString strFontDirectory = GetExecuteDirectory() + L"Fonts";

	char buf[4096];
 	sprintf(buf, "font directory = (%s, .)", H_ASCII_TEXT(strFontDirectory));
	sprintf(buf, "%s, multi-threading=%s", buf, "full");

	HC_Define_System_Options(buf);
}

// 2. CWinApp::OnExitInstance() 처리
void ApplicationManager::ExitInstance()
{
	//:Ken
	delete Wrapper().m_pcHoopsDB;

}

// 실행 파일 경로 (끝에 '\' 붙어서 나옴)
CString ApplicationManager::GetExecuteDirectory()
{
	TCHAR szBuffer[MAX_PATH];
	TCHAR Drive[_MAX_DRIVE];
	TCHAR Path[_MAX_PATH];
	TCHAR Filename[_MAX_FNAME];
	TCHAR Ext[_MAX_EXT];

	GetModuleFileName(NULL, szBuffer, sizeof(szBuffer)); // get process file name
	_wsplitpath_s(szBuffer, Drive, _MAX_DRIVE, Path, _MAX_PATH, Filename, _MAX_FNAME, Ext, _MAX_EXT); // get drive, path, file, ext name

	CString strFilePath;
	strFilePath.Format(L"%s%s", Drive, Path);

	return strFilePath;
}
