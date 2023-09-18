#include "stdafx.h"

#include "Component.Application.h"

#include <Common_Define.h>

#include "../Signal/Signal.h"

// #include "3DF.Signal.Manager.h"
// #include "3DF.Signal.Interface.h"

#include "3DF/3DF.Utility.h"

// ----- HOOPS Header -----
#include <hoops_license.h>

#include <HDB.h>

//:Ken
#include "3DF/Facility.AppOptions.h"

using namespace H3DF;

//Application theApplicationManager;

//== 전달 받은 명령어 분기 =============================================================================

/*
void H3DF::Component::Application::ExecuteSignal(Json::Object & cInObject)
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

		//:Ken
		case Signal::Application::Action::OnDpiAware:
			TheAppOptions.Preference.Session.DpiScale = cInObject.GetReal(SKW_DPISCALE);
			break;

		case Signal::Application::Action::OnUpdatePreference:
			TheAppOptions.Preference.Set(&cInObject);
			break;

		case Signal::Application::Action::OnUpdateFileOption:
			break;

		default:
			assert(false);
			break;
	}
}
*/

//== CWinApp에서 전달되는 메시지 처리 ==================================================================

// 1. Application이 실행될때 최초 처리 CWinApp::InitInstance에서 메시지 전달 받음.
void H3DF::Component::Application::InitInstance()
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

	m_pcHoopsDB = new HDB;
	m_pcHoopsDB->Init();

	// set the font directory
	TCHAR fontDirectory[MAX_PATH + 32];
	::GetWindowsDirectory(fontDirectory, MAX_PATH);
//	
	_tcscat(fontDirectory, _T("\\Fonts"));

	CString strFontDirectory = Utility::GetExecuteDirectory() + L"Fonts";

	char buf[4096];
 	sprintf(buf, "font directory = (%s, .)", H_ASCII_TEXT(strFontDirectory));
	sprintf(buf, "%s, multi-threading=%s", buf, "full");

	HC_Define_System_Options(buf);
}

// 2. CWinApp::OnExitInstance() 처리
void H3DF::Component::Application::ExitInstance()
{
	if (nullptr != m_pcHoopsDB) {
		delete m_pcHoopsDB;
		m_pcHoopsDB = nullptr;
	}
}