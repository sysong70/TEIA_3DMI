#include "stdafx.h"
#include "Component.h"
#include "Layout.h"
#include "Control.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



COLORREF Layout::GetTransparent()
{
	return (COLORREF)Control::EColor::DarkBack;
}
