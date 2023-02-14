#include "stdafx.h"
#include "Dialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



CSize Dialog::ControlGap()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}



int Dialog::FooterPadding()
{
	return globalUtils.ScaleByDPI(6);
}



CSize Dialog::FramePadding()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}



CSize Dialog::WindowPadding()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}
