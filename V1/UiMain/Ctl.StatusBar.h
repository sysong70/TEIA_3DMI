#pragma once

#include "Ctl.h"

//--------------------------------------------------------------------------------------------------

class CtlStatusBar : public CBCGPRibbonStatusBar
{
public:

	CtlStatusBar();

	virtual ~CtlStatusBar();

public:

	bool Initialize(CWnd* pMainFrame);

	void ReceiveSignal(Json::Object* pData);

public:

	void ShowMessage(Json::Object& data);
	// Receive from kernel
	void ShowCoordinate(Json::Object& data);

public:

	void ShowCoordinate(const CString& value);
	// TEST - CtlStatusBar::ShowCoordinate()
	void ShowCoordinate(double x, double y);

	void ShowCoordinate(double x, double y, double z);
};
