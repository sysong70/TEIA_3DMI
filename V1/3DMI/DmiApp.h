// RCT3D.h : main header file for the RCT3D application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include <Json.h>

class RtCommandAnalyzerDll;

class DmiApp : public CBCGPWinApp
{
public:
	DmiApp();

	// Override from CBCGPWorkspace
	virtual void PreLoadState();

	virtual void OnBeforeChangeVisualTheme(CBCGPAppOptions& appOptions, CWnd* pMainWnd);

	bool ExecuteCommand(Json::Object & cInObject);
	bool ExecuteHpsCommand(Json::Object & cInObject);
	bool ExecuteCommand(Json::Object & cInObject, Json::Object & cRetObject);

	void SetMessageCallback(void(CALLBACK * lpfnCallback)(DWORD_PTR *, Json::Object &), DWORD_PTR * pcTarget);

	COleDateTime m_cFileImportStartTime;
	COleDateTime m_cFileImportEndTime;
	COleDateTime m_cHpsStartTime;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	virtual BOOL SaveAllModified();


// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	RtCommandAnalyzerDll * m_pcCommandAnalyzer;
};

extern DmiApp theApp;
