#include "stdafx.h"

#include "Cmd.Base.h"
#include "Wnd.Application.h"
#include "Wnd.MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

void CmdBase::Cancel()
{
	ViewWnd = nullptr;
	TheApp.GetMainFrame().TaskBarCtl.Show(nullptr);
}



void CmdBase::Complete()
{
	ViewWnd = nullptr;
	TheApp.GetMainFrame().TaskBarCtl.Show(nullptr);
}