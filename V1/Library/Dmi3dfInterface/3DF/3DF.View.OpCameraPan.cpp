#include "StdAfx.h"

#include "3DF.View.OpCameraPan.h"

USING_3DF_NAMESPACE

OpCameraPan::OpCameraPan(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraPan(view, DoRepeat, DoCapture)
{

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