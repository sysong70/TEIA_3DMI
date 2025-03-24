#pragma once

#include "Cmd.h"

class CtlTaskPanel;
class WndView;

//--------------------------------------------------------------------------------------------------

class CmdBase
{
public:

	WndView* ViewWnd = nullptr;
	CtlTaskPanel* TaskPanelCtl = nullptr;

public:

	CmdBase() {}

	~CmdBase() {}

public:

	virtual bool ReceiveSignal(Json::Object* pData) { RETURN_FALSE; }

	virtual void Run(WndView* pView = nullptr) { ViewWnd = pView; }

	virtual void Cancel();

	virtual void Complete();
	// run and close immediately
	virtual bool IsRunOnlyOnce() { return false; }
};
