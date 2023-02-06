#include "stdafx.h"
#include "3DFSignal.ApplicationManager.h"

#include <Common_Define.h>

#include "../Signal/Signal.h"
#include "3DFSignal.Manager.h"

// ----- HOOPS Header -----
#include <hoops_license.h>

#include <HDB.h>

USING_3DF_SIGNAL_NAMESPACE

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

	// Thread 설정
	//this is overridden later and could be removed
	HC_Define_System_Options("multi-threading = full");

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
	_tcscat(fontDirectory, _T("\\Fonts"));
	char buf[4096];
 	sprintf(buf, "font directory = (%s, .)", H_ASCII_TEXT(fontDirectory));

	HC_Define_System_Options(buf);
}

// 2. CWinApp::OnExitInstance() 처리
void ApplicationManager::ExitInstance()
{
	//:Ken
	delete Wrapper().m_pcHoopsDB;
}