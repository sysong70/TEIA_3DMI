#pragma once

class CSessionInterfaceApp : public CWinApp
{
public:
	CSessionInterfaceApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
