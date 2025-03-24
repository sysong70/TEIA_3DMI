#pragma once

#include "Cls.DioContainer.h"
#include "Ctl.CommandBar.h"
#include "Ctl.TaskIoBar.h"
#include "Uio.h"

class WndView;

//--------------------------------------------------------------------------------------------------

class ClsUserIoManager
{
public:

	bool UseCommandBar = false;
	bool UseDynamicInput = false;
	bool UseTaskIoBar = true;

	CtlCommandBar CommandBarCtl;
	ClsDioContainer DioContainer;
	CtlTaskIoBar TaskIoBarCtl;

	UserIoParams Params;

public:

	ClsUserIoManager();

	~ClsUserIoManager() {}

	bool Initialize(CWnd* pParentWnd);

	void ReceiveSignal(Json::Object* pData);

	void ViewChanged(WndView* pView);

	void AdjustLayout();

public:

	bool OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	bool OnMouseLeave();

	bool OnMouseMove(UINT nFlags, CPoint point);

public:

	bool PutCommand(Json::Object& data);

	bool PutPrompt(Json::Object& data);

	bool PutEcho(Json::Object& data);

	bool PutError(Json::Object& data);

	bool StandbyCommand(Json::Object& data);
};
