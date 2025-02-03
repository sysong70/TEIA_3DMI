#include "StdAfx.h"

#include "Manager.Session.h"

#include "Manager.Command.h"
#include "Manager.Input.h"

#include "Signal.Connector.h"

#include "Session.h"

#include "../DmiKernel3dInterface/Kernel.Session.h"

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
	bool success = Json::Reader::ReadObject((wchar_t *&)pchBuffer, cInObject);
#ifdef _DEBUG
	// KEN - 20240223, echo data
	if (success == false) {
		Json::Object msg;
		msg.SetString("Echo", CString(pchBuffer));
		Connector::GetInstance(-1).SendData(msg);

		return;
	}
#endif

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
			DEBUG_STOP;
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
	m_cCommandManager.ExecuteApplicationSignal(cInObject);
}

//== View 명령어 처리 부분 ============================================================================

// 1. View 명령어 처리
void SESSION::Manager::Session::ExecuteViewSignal(Json::Object & cInObject)
{
	int nAction = cInObject.GetInteger(SKW_ACTION);
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	SESSION::Session * pcSession = GetSession(nViewId);
	DEBUG_VALID(pcSession);

	//TRACE(L"View Signal: ViewId %d, Action: %d", nViewId, nAction);
	
	if (Signal::View::Action::OnDestruct == (Signal::View::Action)nAction) {
		RemoveSession(nViewId);
	} 
	else {
		pcSession->ExecuteViewSignal(cInObject);
	}
}

// 1-1. Session을 가져옴 (없으면 생성)
SESSION::Session * SESSION::Manager::Session::GetSession(int nViewId)
{
	if (0 > nViewId) {
		return nullptr;
	}

	SESSION::Session * pcSession = m_mpcSessions[nViewId];

	if (nullptr == pcSession) {
		pcSession = new SESSION::Session(&m_cCommandManager, nViewId);
		if (nullptr == pcSession) {
			DEBUG_STOP;
			return nullptr;
		}
		else {
			m_mpcSessions[nViewId] = pcSession;
		}
	}

	// 이 함수를 호출해야 Delivery에 ViewId가 설정됨.
	Connector::GetInstance(nViewId);

	return pcSession;
}

void SESSION::Manager::Session::RemoveSession(int nViewId)
{
	SESSION::Session * pcSession = m_mpcSessions[nViewId];
	m_cCommandManager.RemoveDocView(nViewId);

	if (nullptr != pcSession) {
		delete pcSession;
		m_mpcSessions.erase(nViewId);
	}
}

//== Model Panel 명령어 처리 부분 =====================================================================
void SESSION::Manager::Session::ModelPanelSignal(Json::Object & cInObject)
{
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	SESSION::Session * pcSession = GetSession(nViewId);
	pcSession->ModelPanelSignal(cInObject, nViewId);
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
			pcSession->CommandRequest(cInObject, nViewId);
			break;

		case Signal::TaskBar::Action::OnChangedValue:
			pcSession->CommandChange(cInObject, nViewId);
			break;

		default:
			DEBUG_STOP;
			break;
	}
}