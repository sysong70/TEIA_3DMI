#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"
#include "TracerClient.h"


class CTracerClientApp : public CWinApp
{
public:

	CTracerClientApp() noexcept;


public:

	virtual BOOL InitInstance();
	virtual int ExitInstance();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:

	void Trace(CString message);

private:

	CTracerClient m_tracer;
};

extern CTracerClientApp theApp;
