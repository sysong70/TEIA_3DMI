#include "stdafx.h"
#include "Control.ContextMenu.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

using namespace Control;

BEGIN_MESSAGE_MAP(ContextMenu, CBCGPPopupMenu)
END_MESSAGE_MAP()



Control::ContextMenu::ContextMenu()
{
}



bool Control::ContextMenu::Initailize(CWnd* pParentWnd, CPoint pos, int eDocType)
{
	RETURN_FALSE;
}
