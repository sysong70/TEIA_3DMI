#include <StdAfx.h>

#include "Interface.Session.h"

#include "Signal.Connector.h"

 #include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

KERNEL::Interface::Session theSessionInterface;

using namespace KERNEL;

KERNEL::Interface::Session::Session()
{

}

KERNEL::Interface::Session::~Session()
{

}

//== 명령어 처리 함수 =================================================================================

// 1. 전달받은 신호를 처리하는 부분
void KERNEL::Interface::Session::ExecuteSignal(const wchar_t * pchBuffer)
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
void KERNEL::Interface::Session::ExecuteViewSignal(Json::Object & cObject)
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

void KERNEL::Interface::Session::SetSendSignalFunc(SendSignalFunc pcSendSignalFunc)
{
	m_pcSendSignalFunc = pcSendSignalFunc;
	Connector::SetSender(pcSendSignalFunc);
}
