#include "stdafx.h"

#include "Ast.AppResources.h"
#include "Ctl.TaskPanel.h"
#include "Cmd.Circle2d.h"
#include "Wnd.Application.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View.h"

#include <Signal2d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************
/*
#pragma region Panel

class Circle2dPanel : public CtlTaskPanel
{
public:

	Circle2dPanel(UINT subType, Json::Object* pUiData)
		: CtlTaskPanel(subType, pUiData) {}

public:

	DECLARE_MESSAGE_MAP();
};



BEGIN_MESSAGE_MAP(Circle2dPanel, CWnd)
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

#pragma endregion // REGION
*/
//**************************************************************************************************

void CmdCircle2dTwoPoints::Run(WndView* pView)
{
	RunCommand2d("Circle2d.2Points", DRAW_2D_CMD_Circle_2Points);
}

//**************************************************************************************************

void CmdCircle2dThreePoints::Run(WndView* pView)
{
	RunCommand2d("Circle2d.3Points", DRAW_2D_CMD_Circle_3Points);
}

//**************************************************************************************************

void CmdCircle2dCenterRadius::Run(WndView* pView)
{
	RunCommand2d("Circle2d.CenterRadius", DRAW_2D_CMD_Circle_CenterRadius);
}

//**************************************************************************************************

void CmdCircle2dTwoTangentsRadius::Run(WndView* pView)
{
	RunCommand2d("Circle2d.2TangentsRadius", DRAW_2D_CMD_Circle_2TangentsRadius);
}
