#pragma once

#include "Ctl.DynamicInput.h"

class ClsUserIoManager;
class WndView;

// Dynamic IO controls container
//--------------------------------------------------------------------------------------------------

class ClsDioContainer
{
public:

	bool Valid = false;

	WndView* ViewWnd = nullptr;
	CPoint MousePoint;
	CRect ViewRect;
	HDWP DwpHandle = nullptr;

	CtlDioWrapper LengthCtl;
	CtlDioWrapper AngleCtl;

	CtlDioWrapper PromptCtl;
	CtlDioWrapper CoordXCtl;
	CtlDioWrapper CoordYCtl;
	CtlDioWrapper CoordZCtl;

	ClsUserIoManager& UioManager;

public:

	ClsDioContainer(ClsUserIoManager& manager);

	~ClsDioContainer() {}

	bool Initialize(CWnd* pParentWnd);

	void ReceiveSignal(Json::Object* pData);

	void AdjustLayout();

public:

	bool SetLength(Json::Value* pValue);

	bool SetAngle(Json::Value* pValue);

public:

	bool OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	bool OnMouseLeave();

	bool OnMouseMove(UINT nFlags, CPoint point);

public:

	bool SetCommand() { return false; }

	bool SetPrompt();

	bool SetEcho(const CString& value);

	bool SetError(const CString& value);

	bool StandbyCommand(const CString& prompt);
};
