#include "stdafx.h"
#include "Component.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

int Component::BarHeaderHeight()
{
	return globalUtils.ScaleByDPI(34);
}



int Component::BarFooterHeight()
{
	return globalUtils.ScaleByDPI(34);
}



int Component::BarPadding()
{
	return globalUtils.ScaleByDPI(6);
}
