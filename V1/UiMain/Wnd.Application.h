#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "Wnd.h"

class WndMainFrame;
class WndView;

//--------------------------------------------------------------------------------------------------

class WndApplication : public CBCGPWinApp
{
public:

	friend class WndMainFrame;

	WndApplication();

public: // CWinApp

	CDocument* OpenDocumentFile(LPCTSTR lpszFileName) override;

	CDocument* OpenDocumentFile(LPCTSTR lpszFileName, BOOL bAddToMRU) override;

public:

	WndView* FindView(int id);

	CString GetAppPath(bool bLastBackslash = true);

	WndMainFrame& GetMainFrame() { return *(WndMainFrame*)m_pMainWnd; }

public:

	int ExitInstance() override;

	BOOL InitInstance() override;

	void OnBeforeChangeVisualTheme(CBCGPAppOptions& appOptions, CWnd* pMainWnd) override;

	void PreLoadState() override;

	BOOL SaveAllModified() override;

public:

	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()

public:

	void SetCustomVisualManager();
};



extern WndApplication TheApp;
