#include "StdAfx.h"

#include "Session.h"

#include "Manager.Session.h"

#include "Signal.Connector.h"

#include "../../UiMain/Command.Resource.h"

SESSION::Session::Session(KERNEL::Command::Manager * pcCommandManager, int nId)
{
	DEBUG_VALID(pcCommandManager);
	m_nSessionId = nId;
	m_pcCommandManager = pcCommandManager;
}

SESSION::Session::~Session()
{
}

int SESSION::Session::Id() const
{
	return m_nSessionId;
}

//== View 관련 함수 ==================================================================================

// 1. View 관련 신호 처리
void SESSION::Session::ExecuteViewSignal(Json::Object & cInObject)
{
	DEBUG_VALID(m_pcCommandManager);

	int nAction = cInObject.GetInteger(SKW_ACTION);
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	switch ((Signal::View::Action)nAction)
	{
		case Signal::View::Action::OnInitialize:
			// nViewId를 넣는 이유는 Instnace에서 Signal을 보낼때 식별자로서 ViewId를 보내기 위해서 값을 넣어주는 것임.
			// 실제로는 하나의 Instance를 사용하는 것임.
			ViewInitialize(cInObject, nViewId);

			//:Ken - TEST
			//Connector::GetInstance(-1).application.AddTraceLogV(L"View initialized %d", nViewId);
			//Connector::GetInstance(-1).application.AddTraceLog(L"next message");
			//Connector::GetInstance(-1).application.SaveTraceLog(nullptr);
			//Connector::GetInstance(-1).application.SaveTraceLog(L"c:\\temp\\test.log", false);
			break;

		default:
			m_pcCommandManager->ExecuteViewSignal(cInObject);
			break;
	}
}

// 2. View 초기화, 이 부분에서 ThreadFileOpen 호출.
void SESSION::Session::ViewInitialize(Json::Object & cInObject, int nViewId)
{
	DEBUG_VALID(m_pcCommandManager);
	m_pcCommandManager->ViewInitialize(cInObject, Connector::GetInstance(nViewId));
	m_pcCommandManager->SetThreadFileOpenViewId(nViewId);

	AfxBeginThread(ThreadFileOpen, m_pcCommandManager);
}

// 2-1. Thread File Open
UINT SESSION::Session::ThreadFileOpen(LPVOID pcParam)
{
	KERNEL::Command::Manager * pcCommandManager = static_cast<KERNEL::Command::Manager *>(pcParam);
	pcCommandManager->ThreadFileOpen();

	return 0;
}

/*
//== Command 관련 함수 ===============================================================================
void SESSION::Session::ViewCancelCommands()
{
	m_pcDocView->CancelCommands();
}
*/

//== Command 관련 함수 ===============================================================================

void SESSION::Session::CommandRequest(Json::Object & cInObject, int nViewId)
{
	DEBUG_VALID(m_pcCommandManager);
	m_pcCommandManager->CommandRequest(cInObject, nViewId);
}

void SESSION::Session::CommandChange(Json::Object & cInObject, int nViewId)
{
	DEBUG_VALID(m_pcCommandManager);
	m_pcCommandManager->CommandChange(cInObject, nViewId);
}

//== Panel 관련 함수 =================================================================================
void SESSION::Session::ModelPanelSignal(Json::Object & cInObject, int nViewId)
{
	DEBUG_VALID(m_pcCommandManager);
	m_pcCommandManager->ModelPanelSignal(cInObject, nViewId);
}

