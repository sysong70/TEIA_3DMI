#include "stdafx.h"

#include "Cmd.Test.h"
#include "Ctl.TaskPanel.h"
#include "Ast.AppResources.h"
#include "Wnd.Application.h"
#include "Wnd.MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#pragma region Panel

class TestPanel : public CtlTaskPanel
{
public:

	TestPanel(Json::Object* pUiData)
		: CtlTaskPanel(CUSTOM_3D_CMD_KEN_Test9, pUiData) {}

	~TestPanel() override {}

public:

	void ConstructBody() override
	{
		if (PropListCtl.Initialize(this, WM_USER) == false) {
			DEBUG_RETURN;
		}

		PropListCtl.InitializeDesign(GetUiData().GetAt("properties"));
	}

	DECLARE_MESSAGE_MAP();
};



BEGIN_MESSAGE_MAP(TestPanel, CWnd)
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

#pragma endregion // REGION

//**************************************************************************************************

bool CmdTest9::ReceiveSignal(Json::Object* pData)
{
	DEBUG_VALID(ViewWnd);
	REMOVE_POINTER(pData);

	return true;
}



void CmdTest9::Run(WndView* pView)
{
	__super::Run(pView);

	CtlTaskBar& taskBar = TheApp.GetMainFrame().TaskBarCtl;
	Json::Object& uiData = TheAppResources.GetTask("Test");

	TaskPanelCtl = new TestPanel(&uiData);
	TaskPanelCtl->Initialize(&taskBar);

	taskBar.SetPanel(TaskPanelCtl);
	taskBar.Show(ViewWnd);
}
