#include "stdafx.h"
#include "Dialog.h"



CSize Dialog::ControlGap()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}



int Dialog::FooterPadding()
{
	return globalUtils.ScaleByDPI(6);
}
