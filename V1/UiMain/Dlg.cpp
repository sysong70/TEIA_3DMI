#include "stdafx.h"

#include "Dlg.Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

CSize Dlg::FramePadding()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}



CSize Dlg::WindowPadding()
{
	return globalUtils.ScaleByDPI(CSize(6, 6));
}
