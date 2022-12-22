#include "stdafx.h"
#include "DmiHpsInterface.h"

#include "DmiHpsSignalAnalyzer.h"

#include "../Signal/Signal.h"
#include "Common_Define.h"

#include <Json.h>

#define CHECK_HPS_VISUALIZE if(nullptr == m_pcHpsCommandAnalyzer) { assert(FALSE);  return false; } 

using namespace Json;

class DmiHpsSignalAnalyzer;

DmiHpsInterface::DmiHpsInterface()
{
	m_pcSignalAnalyzer = (DWORD_PTR *) new DmiHpsSignalAnalyzer(this);
	if(nullptr == m_pcSignalAnalyzer) {
		assert(false);
	}
}

DmiHpsInterface::~DmiHpsInterface()
{
	if(nullptr == m_pcSignalAnalyzer) {
		delete m_pcSignalAnalyzer;
		m_pcSignalAnalyzer = nullptr;
	}
}

void DmiHpsInterface::ExecuteSignal(Json::Object & cInObject)
{
	assert(m_pcSignalAnalyzer);
	((DmiHpsSignalAnalyzer *)m_pcSignalAnalyzer)->ExecuteSignal(cInObject);
}

// Callback 함수를 지정한 MainFrame으로 메시지 전달
bool DmiHpsInterface::SendExecuteCommand(Json::Object & cObject)
{
	if(nullptr == m_lpfnCommandAnalyzerExecuteCommandCallBack) {
		return false;
	}

	return m_lpfnCommandAnalyzerExecuteCommandCallBack(cObject);
}

void DmiHpsInterface::SetExecuteCommandCallBack(bool(CALLBACK * lpfnCallback)(Json::Object & cObject))
{
	m_lpfnCommandAnalyzerExecuteCommandCallBack = lpfnCallback;
}

void DmiHpsInterface::SendCommandCallback(Json::Object & cObject)
{
	m_lpfnCommandCallback(m_pcTarget, cObject);
}

void DmiHpsInterface::SetCommandCallback(void(CALLBACK * lpfnCallback)(DWORD_PTR *, Json::Object &), DWORD_PTR * pcTarget)
{
	m_pcTarget = pcTarget;
	m_lpfnCommandCallback = lpfnCallback;
}