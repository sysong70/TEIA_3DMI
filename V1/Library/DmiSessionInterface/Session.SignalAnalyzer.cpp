#include "stdafx.h"

#include "Session.SignalAnalyzer.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

using namespace Json;
using namespace std::chrono;

#define MSG_FILE_IMPORT_INFORMATION				(WM_USER + 1)

SESSION::SignalAnalyzer theSignalAnalyzer;

SESSION::SignalAnalyzer::SignalAnalyzer()
{
}

//== 명령어 처리 부분 =================================================================================

// 1. 최초 명령어 처리 분기
void SESSION::SignalAnalyzer::ExecuteSignal(const wchar_t * pchBuffer)
{
	Json::Object cObject;
	Reader::ReadObject((wchar_t *&) pchBuffer, cObject);

	//CString strText = cObject.ToString() + L"\r\n";
	//OutputDebugString(strText);

	int nTarget = cObject.GetInteger(SKW_TARGET);

	switch((Signal::Target)nTarget)
	{
		case Signal::Target::Application: // CApp
		case Signal::Target::View:
			m_3DF_Interface.ExecuteSignal(cObject);
			break;

		case Signal::Target::ViewPanel:
		case Signal::Target::LayerPanel:
		case Signal::Target::ScenePanel:
		case Signal::Target::ModelPanel:
			break;

		default:
			assert(false);
			break;
	}
}

void SESSION::SignalAnalyzer::SetSendSignalFunc(SendSignalFunc pcSendSignalFunc)
{
	m_pcSendSignalFunc = pcSendSignalFunc;

	//m_cHpsInterface.SetSignalCallback(lpfnSignalCallback);
	m_3DF_Interface.SetSignalCallback(pcSendSignalFunc);
}