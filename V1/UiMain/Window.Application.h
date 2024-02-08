#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "Window.h"
#include "Window.MainFrame.h"



namespace Window
{
	class Application : public CBCGPWinApp
	{
	public:

		friend class MainFrame;
		friend class View;

		Application();

		MainFrame& GetMainFrame();

		View* FindView(int id);

		CString GetAppPath(bool bLastBackslash = true);

	public:

		int ExitInstance() override;

		BOOL InitInstance() override;

		void OnBeforeChangeVisualTheme(CBCGPAppOptions& appOptions, CWnd* pMainWnd) override;

		void PreLoadState() override;

		BOOL SaveAllModified() override;

	public:

		afx_msg void OnAppAbout();

		DECLARE_MESSAGE_MAP()

	private:

		void SetCustomVisualManager();
	};
}

extern Window::Application TheApplication;
