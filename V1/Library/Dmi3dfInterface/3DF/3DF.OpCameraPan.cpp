#include "StdAfx.h"

#include "3DF.OpCameraPan.h"

USING_3DF_NAMESPACE

OpCameraPan::OpCameraPan(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraPan(view, DoRepeat, DoCapture)
{

}

const char * OpCameraPan::GetName()
{
	return "3DF_OpCameraPan";
}

HBaseOperator * OpCameraPan::Clone()
{
	return new OpCameraPan(GetView());
}

int OpCameraPan::OnRButtonDown(HEventInfo & hevent)
{
	return HOpCameraPan::OnLButtonDown(hevent);
}

int OpCameraPan::OnRButtonDownAndMove(HEventInfo & hevent)
{
	return HOpCameraPan::OnLButtonDownAndMove(hevent);
}

int OpCameraPan::OnRButtonUp(HEventInfo & hevent)
{
	return HOpCameraPan::OnLButtonUp(hevent);
}