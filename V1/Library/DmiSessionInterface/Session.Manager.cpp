#include <StdAfx.h>

#include "Session.Manager.h"

#include "Session.h"
#include "Session.Signal.Connector.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

SESSION::Manager theSessionManager;

using namespace SESSION;

SESSION::Manager::Manager()
{

}

//== 신호 처리 함수 ==================================================================================

// 전달 받은 wchar 문자열을 Json::Object로 변환하여 신호를 처리한다.
void SESSION::Manager::ExecuteSignal(const wchar_t * pchBuffer)
{
	Json::Object cObject;
	Json::Reader::ReadObject((wchar_t *&)pchBuffer, cObject);

	//CString strText = cObject.ToString() + L"\r\n";
	//OutputDebugString(strText);

	int nViewId = cObject.GetInteger(SKW_VIEWID);

	Session * pcSession = m_mpcSessions[nViewId];

	if (nullptr == pcSession) {
		pcSession = new Session();
		if (nullptr == pcSession) {
			assert(false);
			return;
		}
	}

	m_mpcSessions[nViewId] = pcSession;

	pcSession->ExecuteSignal(cObject);
}

void SESSION::Manager::SetSendSignalFunc(SendSignalFunc pcSendSignalFunc)
{
	m_pcSendSignalFunc = pcSendSignalFunc;

	Connector::SetSender(pcSendSignalFunc);
}