#include <StdAfx.h>

#include "Manager.Session.h"

#include "Manager.Command.h"
#include "Manager.Input.h"

#include "Signal.Connector.h"

#include "Session.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

SESSION::Manager::Session theSessionManager;

using namespace SESSION;

SESSION::Manager::Session::Session()
{
	theInputManager.SetSessionManager(this);

	theCommandManager.SetSessionManager(this);
}

SESSION::Manager::Session::~Session()
{
}

SESSION::Manager::Type SESSION::Manager::Session::Type()
{
	return SESSION::Manager::Type::Session;
}

//== 명령어 처리 부분 =================================================================================

// 1. 기본 신호 처리 (여기에서 신호 분기가 시작됨.)
void SESSION::Manager::Session::ExecuteSignal(const wchar_t * pchBuffer)
{
/*
	size_t nBufferSize = wcslen(pchBuffer) + 1; // 널 종료 문자('\0')를 포함해서 크기 계산

	// 대상 문자열에 충분한 메모리 할당
	wchar_t * pchCopyBuffer = new wchar_t[nBufferSize];
	if (nullptr == pchCopyBuffer) {
		wprintf(L"메모리 할당 실패\n");
		return;
	}

	// 문자열 복사
	wcscpy(pchCopyBuffer, pchBuffer);
*/

	Json::Object cInObject;
	// ReadObject에 buffer에 내용을 전달하고 나오면 buffer는 empty됨.
	Json::Reader::ReadObject((wchar_t *&)pchBuffer, cInObject);

	// 들어오는 값을 순서대로 처리하도록 한다.
	int nTarget = cInObject.GetInteger(SKW_TARGET);

	switch ((Signal::Target)nTarget)
	{
		case Signal::Target::Application:
			ExecuteApplicationSignal(cInObject);
			break;

		case Signal::Target::View:
			ExecuteViewSignal(cInObject);
			break;

		case Signal::Target::ModelPanel:
			ModelPanelSignal(cInObject);
			break;

		case Signal::Target::TaskBar:
			ExecuteCommand(cInObject);
			break;

		default:
			assert(false);
			break;
	}
}

void SESSION::Manager::Session::SetSendSignalFunc(SendSignalFunc lpfnSignalCallback)
{
	m_pcSendSignal = lpfnSignalCallback;
	Connector::SetSender(lpfnSignalCallback);
}

//== Application 명령어 처리 부분 =====================================================================

// 1. Application 명령어 처리
// Application은 최초에 설정되는 값이기 때문에 저장하고 있다가 처리하도록 한다.
void SESSION::Manager::Session::ExecuteApplicationSignal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);

	switch ((Signal::Application::Action)nAction)
	{
		case Signal::Application::Action::OnInitInstance:
			m_cApplication.InitInstance();
			break;

		case Signal::Application::Action::OnExitInstance:
			m_cApplication.ExitInstance();
			break;

		case Signal::Application::Action::OnDpiAware:
			break;

		case Signal::Application::Action::OnUpdatePreference: {
			//:TODO
		} break;

		case Signal::Application::Action::OnUpdateFileOption: {
			//:TODO
			// H3DX - TheFileOptions.Set(cInObject.GetAt(SKW_VALUE));
		} break;

		case Signal::Application::Action::OnFileOptionReference: {
			//:TODO
			// H3DX - TheFileOptions.SetReference(cInObject.GetAt(SKW_VALUE));
		} break;

		default:
			break;
	}
}

//== View 명령어 처리 부분 ============================================================================

// 1. View 명령어 처리
void SESSION::Manager::Session::ExecuteViewSignal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	SESSION::Session * pcSession = GetSession(nViewId);
	
	switch ((Signal::View::Action)nAction)
	{
		case Signal::View::Action::OnInitialize:
			// nViewId를 넣는 이유는 Instnace에서 Signal을 보낼때 식별자로서 ViewId를 보내기 위해서 값을 넣어주는 것임.
			// 실제로는 하나의 Instance를 사용하는 것임.
			pcSession->ViewInitialize(cInObject, Connector::GetInstance(nViewId));
			//:Ken - TEST
			//Connector::GetInstance(-1).application.AddTraceLogV(L"View initialized %d", nViewId);
			//Connector::GetInstance(-1).application.AddTraceLog(L"next message");
			//Connector::GetInstance(-1).application.SaveTraceLog(nullptr);
			//Connector::GetInstance(-1).application.SaveTraceLog(L"c:\\temp\\test.log", false);
			break;

		case Signal::View::Action::OnConstruct:
			break;

		case Signal::View::Action::OnDestruct:
			pcSession->ViewDestruct();
			break;

		case Signal::View::Action::OnPaint:
			pcSession->ViewPaint(cInObject);
			break;

		case Signal::View::Action::OnResize:
			// Resize를 하면 속도 저하가 발생함. 그리고 할 필요가 없음.
			//pcSession->ViewResize(cInObject);
			break;

		case Signal::View::Action::OnMouseMove:
		case Signal::View::Action::OnLButtonDown:
		case Signal::View::Action::OnLButtonUp:
		case Signal::View::Action::OnMButtonDown:
		case Signal::View::Action::OnMButtonUp:
		case Signal::View::Action::OnRButtonDown:
		case Signal::View::Action::OnRButtonUp:
		case Signal::View::Action::OnMouseWheel:
			pcSession->ViewMouseSignal(cInObject);
			break;

			//:Ken - 20230607
		case Signal::View::Action::OnInput:
		case Signal::View::Action::OnChar:
		case Signal::View::Action::OnKeyDown:
		case Signal::View::Action::OnKeyUp:
			pcSession->ViewKeyboardSignal(cInObject);
			break;

		case Signal::View::Action::OnCancel:
			pcSession->ViewCancelCommands();
			break;

		case Signal::View::Action::OnCommand:
			pcSession->ViewExecuteCommand(cInObject);
			break;

		default:
			assert(false);
			break;
	}
}

// 1-1. Session을 가져옴 (없으면 생성)
SESSION::Session * SESSION::Manager::Session::GetSession(int nViewId)
{
	SESSION::Session * pcSession = m_mpcSessions[nViewId];

	if (nullptr == pcSession) {
		pcSession = new SESSION::Session();
		pcSession->SessionId(nViewId);
		if (nullptr == pcSession) {
			assert(false);
			return nullptr;
		}
		else {
			m_mpcSessions[nViewId] = pcSession;
		}
	}

	Connector::GetInstance(nViewId);

	return pcSession;
}

//== Model Panel 명령어 처리 부분 =====================================================================
void SESSION::Manager::Session::ModelPanelSignal(Json::Object & cInObject)
{
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	SESSION::Session * pcSession = GetSession(nViewId);
	pcSession->ModelPanelSignal(cInObject);
}

//== Command 명령어 처리 부분 =========================================================================
void SESSION::Manager::Session::ExecuteCommand(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);
	int nViewId = cInObject.GetInteger(SKW_VIEWID);
	int nId = cInObject.GetInteger(SKW_ID);

	SESSION::Session * pcSession = GetSession(nViewId);

	switch ((Signal::TaskBar::Action) nAction)
	{
		case Signal::TaskBar::Action::OnRequestValue:
			pcSession->CommandRequest(cInObject);
			break;

		case Signal::TaskBar::Action::OnChangedValue:
			pcSession->CommandChange(cInObject);
			break;

		default:
			assert(false);
			break;
	}
}