#include <StdAfx.h>

#include "Manager.Session.h"

#include "Kernel.Signal.Connector.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

KERNEL::Manager::Session theSessionManager;

using namespace KERNEL;

KERNEL::Manager::Session::Session()
{

}

KERNEL::Manager::Session::~Session()
{

}

//== 명령어 처리 함수 =================================================================================

// 1. 전달받은 신호를 처리하는 부분
void KERNEL::Manager::Session::ExecuteSignal(const wchar_t * pchBuffer)
{
	Json::Object cObject;
	Json::Reader::ReadObject((wchar_t *&)pchBuffer, cObject);

	int nTarget = cObject.GetInteger(SKW_TARGET);

	switch ((Signal::Target)nTarget)
	{
		case Signal::Target::Application:
			m_cApplication.ExecuteSignal(cObject);
			break;

		case Signal::Target::View:
			ExecuteViewSignal(cObject);
			break;
	}

	// Target값을 이용해서, Application인지 View인지 여부를 확인한다.
}


// 1-1. 전달받은 View Signal을 처리한다.
// Map에서 View Id를 검색하고 없는 경우에 생성하고 있는 경우에는 Signal을 전달한다.
void KERNEL::Manager::Session::ExecuteViewSignal(Json::Object & cObject)
{
	int nViewId = cObject.GetInteger(SKW_VIEWID);

	View * pcView = m_mpcViews[nViewId];

	if (nullptr == pcView) {
		pcView = new View();
		if (nullptr == pcView) {
			assert(false);
			return;
		}

		m_mpcViews[nViewId] = pcView;
	}

	pcView->ExecuteSignal(cObject);
}

void KERNEL::Manager::Session::SetSendSignalFunc(SendSignalFunc pcSendSignalFunc)
{
	m_pcSendSignalFunc = pcSendSignalFunc;
	Connector::SetSender(pcSendSignalFunc);
}
