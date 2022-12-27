#include "stdafx.h"
#include "Component.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



int Component::TreeRowHeight()
{
	return globalUtils.ScaleByDPI(24);
}



int Component::PropListRowPadding()
{
	return globalUtils.ScaleByDPI(1);
}
