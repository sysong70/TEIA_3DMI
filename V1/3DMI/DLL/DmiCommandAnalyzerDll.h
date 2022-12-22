#pragma once

#include "DllHelper.h"

#include <DmiCommandAnalyzer/DLL/DmiCommandAnalyzerDll.h>

class RtCommandAnalyzerDll
{
public:
	RtCommandAnalyzerDll();

protected:
	DllHelper m_cDLL{ L"3DMICommandAnalyzer.dll" };

public:
	decltype(ExecuteCommand) * ExecuteCommand = m_cDLL["ExecuteCommand"];
	decltype(SetCommandCallback) * SetCommandCallback = m_cDLL["SetCommandCallback"];
	
	// 삭제 예정 함수
// 	decltype(InitDisplay) * InitDisplay = m_cDLL["InitDisplay"];
// 	decltype(ExitDisplay) * ExitDisplay = m_cDLL["ExitDisplay"];
	decltype(ExecuteCommandType1) * ExecuteCommandType1 = m_cDLL["ExecuteCommandType1"];
	decltype(ExecuteCommandType2) * ExecuteCommandType2 = m_cDLL["ExecuteCommandType2"];
	decltype(SetMainFrameSendMessageCallback) * SetMainFrameSendMessageCallback = m_cDLL["SetMainFrameSendMessageCallback"];
	decltype(SetMessageCallback) * SetMessageCallback = m_cDLL["SetMessageCallback"];
};