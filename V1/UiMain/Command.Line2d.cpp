#include "stdafx.h"
#include "Command.Line2d.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

bool Command::Line2d::TwoPoints::ReceiveSignal(Json::Object* pData)
{
	return true;
}



void Command::Line2d::TwoPoints::Run(Window::View* pView)
{
	RunCommand("Line2d.2Points", DRAW_2D_CMD_Line);
}

//**************************************************************************************************

bool Command::Line2d::Polyline::ReceiveSignal(Json::Object* pData)
{
	return false;
}



void Command::Line2d::Polyline::Run(Window::View* pView)
{
	RunCommand("Line2d.Polyline", DRAW_2D_CMD_Polyline);
}

//**************************************************************************************************

bool Command::Line2d::Rectangle::ReceiveSignal(Json::Object* pData)
{
	return false;
}



void Command::Line2d::Rectangle::Run(Window::View* pView)
{
	RunCommand("Line2d.Rectangle", DRAW_2D_CMD_Rectangle);
}

//**************************************************************************************************

bool Command::Line2d::Polygon::ReceiveSignal(Json::Object* pData)
{
	return false;
}



void Command::Line2d::Polygon::Run(Window::View* pView)
{
	RunCommand("Line2d.Polygon", DRAW_2D_CMD_Polygon);
}
