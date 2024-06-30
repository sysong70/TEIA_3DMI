#include "StdAfx.h"

#include "Command.Manager.h"
#include "Impl/Command.ManagerImpl.h"

#include "Kernel.Application.h"

#include <3DF/3DF.h>
#include <3DF/Math.h>
#include <3DF/Window.h>

#include "../../../UiMain/Command.Resource.h"

using namespace KERNEL;

KERNEL::Command::Manager::Manager()
{
	m_pcImpl = new KERNEL::Command::ManagerImpl();
}

void KERNEL::Command::Manager::ExecuteApplicationSignal(Json::Object & cInObject)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	int nAction = cInObject.GetInteger(SKW_ACTION);

	switch ((Signal::Application::Action)nAction)
	{
		case Signal::Application::Action::OnInitInstance:
			pcImpl->m_cApplication.InitInstance();
			break;

		case Signal::Application::Action::OnExitInstance:
			pcImpl->m_cApplication.ExitInstance();
			break;

		case Signal::Application::Action::OnDpiAware: {
// 			double dDpiScale = cInObject.GetReal(SKW_DPISCALE);
// 			pcImpl->m_cApplication.OnDpiAware(dDpiScale);
		} break;

		case Signal::Application::Action::OnUpdatePreference:
			//:TODO
			break;

			//:Ken - 20240131
		case Signal::Application::Action::OnUpdateFileOption:
			pcImpl->m_cApplication.OnUpdateFileOption(cInObject.GetAt(SKW_VALUE));
			break;

			//:Ken - 20240131
		case Signal::Application::Action::OnFileOptionReference:
			pcImpl->m_cApplication.OnFileOptionReference(cInObject.GetAt(SKW_VALUE));
			break;

		default:
			break;
	}
}

//== View 관련 함수 ==================================================================================

// 1. View 관련 신호 처리
void KERNEL::Command::Manager::ExecuteViewSignal(Json::Object & cInObject)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	int nAction = cInObject.GetInteger(SKW_ACTION);
	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	TRACE(L"ExecuteViewSignal Action: %d, ViewId: %d", nAction, nViewId);

	switch ((Signal::View::Action)nAction)
	{
		case Signal::View::Action::OnInitialize:
			// nViewId를 넣는 이유는 Instnace에서 Signal을 보낼때 식별자로서 ViewId를 보내기 위해서 값을 넣어주는 것임.
			// 실제로는 하나의 Instance를 사용하는 것임.
			ViewInitialize(cInObject, Connector::GetInstance(nViewId));

			//:Ken - TEST
			//Connector::GetInstance(-1).application.AddTraceLogV(L"View initialized %d", nViewId);
			//Connector::GetInstance(-1).application.AddTraceLog(L"next message");
			//Connector::GetInstance(-1).application.SaveTraceLog(nullptr);
			//Connector::GetInstance(-1).application.SaveTraceLog(L"c:\\temp\\test.log", false);
			break;

		case Signal::View::Action::OnConstruct:
			break;

		case Signal::View::Action::OnPaint:
			ViewPaint(cInObject, nViewId);
			//:Ken - TEST
			//Connector::GetInstance(nViewId).view.PaintOverlap();
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
			pcImpl->ViewMouseSignal(cInObject, nViewId);
			break;

			//:Ken - 20230607
		case Signal::View::Action::OnInput:
		case Signal::View::Action::OnChar:
		case Signal::View::Action::OnKeyDown:
		case Signal::View::Action::OnKeyUp:
			pcImpl->SessionKeyboardSignal(cInObject, nViewId);
			break;

		case Signal::View::Action::OnCancel:
			//ViewCancelCommands();
			break;

		case Signal::View::Action::OnCommand:
			pcImpl->SessionExecuteCommand(cInObject, nViewId);
			break;

		default:
			DEBUG_STOP;
			break;
	}
}

// 2. View 초기화, 이 부분에서 ThreadFileOpen 호출.
void KERNEL::Command::Manager::ViewInitialize(Json::Object & cInObject, Signal::Delivery & cInstance)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	int nViewId = cInObject.GetInteger(SKW_VIEWID);

	Session * pcSession = pcImpl->GetSession(nViewId);
	DEBUG_VALID(pcSession);

	pcSession->SetDelivery(cInstance);
	pcSession->Initialize(cInObject);
}

void KERNEL::Command::Manager::ThreadFileOpen()
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	int nViewId = pcImpl->m_nThreadFileOpenViewId;
	if (-1 == nViewId) {
		DEBUG_STOP;
		return;
	}

	Session * pcSession = pcImpl->GetSession(nViewId);
	DEBUG_VALID(pcSession);

	pcSession->ThreadFileOpen();

	pcImpl->m_nThreadFileOpenViewId = -1;
}

void KERNEL::Command::Manager::RemoveDocView(int nId)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_cApplication.RemoveDocView(nId);
}

void KERNEL::Command::Manager::SetThreadFileOpenViewId(int nId)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_nThreadFileOpenViewId = nId;
}

void KERNEL::Command::Manager::CommandRequest(Json::Object & cInObject, int nViewId)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	Session * pcSession = pcImpl->GetSession(nViewId);
	DEBUG_VALID(pcSession);

	pcSession->CommandRequest(cInObject);
}

// Command가 변경되는 경우 처리
void KERNEL::Command::Manager::CommandChange(Json::Object & cInObject, int nViewId)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	int nId = cInObject.GetInteger(SKW_ID);

	Session * pcSession = pcImpl->GetSession(nViewId);
	DEBUG_VALID(pcSession);

	pcSession->CommandChange(cInObject);
}

void KERNEL::Command::Manager::ModelPanelSignal(Json::Object & cInObject, int nViewId)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	Session * pcSession = pcImpl->GetSession(nViewId);
	DEBUG_VALID(pcSession);

	pcSession->ModelPanelSignal(cInObject);
}

void KERNEL::Command::Manager::ViewPaint(Json::Object & cInObject, int nViewId)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	Session * pcSession = pcImpl->GetSession(nViewId);
	DEBUG_VALID(pcSession);

	pcSession->Paint(cInObject);
}

void KERNEL::Command::Manager::ViewResize(Json::Object & cInObject, int nViewId)
{
	ManagerImpl * pcImpl = (ManagerImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	Session * pcSession = pcImpl->GetSession(nViewId);
	DEBUG_VALID(pcSession);

	pcSession->Resize(cInObject);
}