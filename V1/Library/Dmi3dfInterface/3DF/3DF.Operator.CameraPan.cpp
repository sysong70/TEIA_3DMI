#include "StdAfx.h"

#include "3DF.Operator.CameraPan.h"

USING_3DF_NAMESPACE

Operator::CameraPan::CameraPan(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraPan(view, DoRepeat, DoCapture)
{

}

const char * Operator::CameraPan::GetName()
{
	return "3DF_OpCameraPan";
}

HBaseOperator * Operator::CameraPan::Clone()
{
	return new CameraPan(GetView());
}

int Operator::CameraPan::OnRButtonDown(HEventInfo & hevent)
{
	return HOpCameraPan::OnLButtonDown(hevent);
}

int Operator::CameraPan::OnRButtonDownAndMove(HEventInfo & hevent)
{
	return HOpCameraPan::OnLButtonDownAndMove(hevent);
}

int Operator::CameraPan::OnRButtonUp(HEventInfo & hevent)
{
	return HOpCameraPan::OnLButtonUp(hevent);
}