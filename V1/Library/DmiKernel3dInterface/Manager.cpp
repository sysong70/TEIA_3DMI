#include <StdAfx.h>

#include "Manager.h"

#include "Kernel.h"
#include "Signal.Connector.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

KERNEL::Manager theKernelManager;

using namespace KERNEL;

KERNEL::Manager::Manager()
{

}

//== 신호 처리 함수 ==================================================================================

// 전달 받은 wchar 문자열을 Json::Object로 변환하여 신호를 처리한다.
void KERNEL::Manager::ExecuteSignal(const wchar_t * pchBuffer)
{
	Json::Object cObject;
	Json::Reader::ReadObject((wchar_t *&)pchBuffer, cObject);

	//CString strText = cObject.ToString() + L"\r\n";
	//OutputDebugString(strText);

	int nViewId = cObject.GetInteger(SKW_VIEWID);

	Kernel * pcSession = m_mpcSessions[nViewId];

	if (nullptr == pcSession) {
		pcSession = new Kernel();
		if (nullptr == pcSession) {
			assert(false);
			return;
		}
	}

	m_mpcSessions[nViewId] = pcSession;

	pcSession->ExecuteSignal(cObject);
}

void KERNEL::Manager::SetSendSignalFunc(SendSignalFunc pcSendSignalFunc)
{
	m_pcSendSignalFunc = pcSendSignalFunc;

	Connector::SetSender(pcSendSignalFunc);
}