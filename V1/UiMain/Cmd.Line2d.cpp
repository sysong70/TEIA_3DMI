#include "stdafx.h"

#include "Ast.AppResources.h"
#include "Cmd.Line2d.h"
#include "Ctl.TaskPanel.h"
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

void CmdLine2dTwoPoints::Run(WndView* pView)
{
	RunCommand2d("Line2d.2Points", DRAW_2D_CMD_Line);
}

//**************************************************************************************************

void CmdLine2dPolyline::Run(WndView* pView)
{
	RunCommand2d("Line2d.Polyline", DRAW_2D_CMD_Polyline);
}

//**************************************************************************************************

void CmdLine2dRectangle::Run(WndView* pView)
{
	RunCommand2d("Line2d.Rectangle", DRAW_2D_CMD_Rectangle);
}

//**************************************************************************************************

void CmdLine2dPolygon::Run(WndView* pView)
{
	RunCommand2d("Line2d.Polygon", DRAW_2D_CMD_Polygon);
}
