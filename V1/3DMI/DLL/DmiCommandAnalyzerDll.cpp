#include "pch.h"

#include "DmiCommandAnalyzerDll.h"

_declspec (dllexport) void SetMainFrameSendMessageCallback(LRESULT (CALLBACK * lpfnCallback)(UINT, WPARAM, LPARAM));

typedef bool(*CallBackMainFrameSendMessage)(LRESULT (CALLBACK * lpfnCallback)(UINT, WPARAM, LPARAM));

// #callback : MainFrameSendMessage
LRESULT CALLBACK MainFrameSendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	CWinApp * pcApp = AfxGetApp();// ->GetMainWnd();
	CWnd * pcMainFrame = AfxGetMainWnd();

	return AfxGetApp()->GetMainWnd()->SendMessage(message, wParam, lParam);
}

RtCommandAnalyzerDll::RtCommandAnalyzerDll()
{
	// #callback
	SetMainFrameSendMessageCallback(MainFrameSendMessage);
//	cDLLSheet(CallBackTest);
}