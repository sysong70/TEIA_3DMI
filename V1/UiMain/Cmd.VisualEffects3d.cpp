#include "stdafx.h"

#include "Ast.AppResources.h"
#include "Cmd.Indexer.h"
#include "Cmd.VisualEffects3d.h"
#include "Ctl.TaskPanel.h"
#include "Wnd.Application.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View.h"

#include <Signal3d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define theDelivery	(*taskBar.GetView()->GetDelivery3d())



namespace
{
	const UINT CommandId = HOME_3D_LST_VisualEffects;
	const CStringA TaskName = "VisualEffects3d";

	enum class Id
	{
		ToolBar = WM_USER + 1,
		PropList,
	};
}

//**************************************************************************************************

#pragma region Panel

class VisualEffects3dPanel : public CtlTaskPanel
{
public:

	VisualEffects3dPanel(Json::Object* pUiData)
		: CtlTaskPanel(CommandId, pUiData) {}

public:

	void ConstructBody() override
	{
		if (PropListCtl.Initialize(this, (UINT)Id::PropList) == false) {
			DEBUG_RETURN;
		}

		PropListCtl.InitializeDesign(GetUiData().GetAt("properties"));
	}

	void ConstructHeader() override
	{
		ToolBarCtl.SetPivot(Ctl::EPivot::TopLeft);
		ToolBarCtl.Initialize(this, (UINT)Id::ToolBar);

		ToolBarCtl.AddButton(HOME_3D_CMD_ViewStyle_Shade);
		ToolBarCtl.AddButton(HOME_3D_CMD_ViewStyle_ShadeWithEdges);
		ToolBarCtl.AddButton(HOME_3D_CMD_ViewStyle_Wireframe);
		ToolBarCtl.AddButton(HOME_3D_CMD_ViewStyle_HiddenLineRemove);
		ToolBarCtl.AddButton(HOME_3D_CMD_ViewStyle_Tessellated);

		ToolBarCtl.SetCheck(TheCmdActive.Home.VisualEffects3d, true);
	}

public:

	afx_msg void OnCommand(UINT id)
	{
		ToolBarCtl.SetCheck(id, true);
		TheCmdActive.Home.VisualEffects3d = id;

		CtlTaskBar& taskBar = *(CtlTaskBar*)GetParent();
		theDelivery.view.OnCommand(id);

		PropListCtl.SetCurSel(NULL);

		if (id == HOME_3D_CMD_ViewStyle_HiddenLineRemove) {
			PropListCtl.Show(L"Shadow", false);
			PropListCtl.Show(L"PlaneReflection", false);
			PropListCtl.Show(L"AmbientOcclusion", true);
			PropListCtl.Show(L"SilhouetteEdges", true);
			PropListCtl.Show(L"Bloom", false);

			PropListCtl.ExpandAll(true);
			PropListCtl.EnableWindow(TRUE);
		}
		else if (id == HOME_3D_CMD_ViewStyle_Wireframe) {
			PropListCtl.ShowAll(true);
			PropListCtl.ExpandAll(false);
			PropListCtl.EnableWindow(FALSE);
		}
		else {
			PropListCtl.ShowAll(true);
			PropListCtl.ExpandAll(true);
			PropListCtl.EnableWindow(TRUE);
		}
	}

	DECLARE_MESSAGE_MAP();
};



BEGIN_MESSAGE_MAP(VisualEffects3dPanel, CWnd)
	ON_WM_SIZE()
	ON_COMMAND_RANGE(HOME_3D_CMD_ViewStyle_Shade, HOME_3D_CMD_ViewStyle_Tessellated, OnCommand)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

#pragma endregion // REGION

//**************************************************************************************************

bool CmdVisualEffects3d::ReceiveSignal(Json::Object* pData)
{
	DEBUG_VALID(ViewWnd);

	Json::Object& data = *pData;
	if (data.GetInteger(SKW_ID) != CommandId) {
		REMOVE_POINTER(pData);
		RETURN_FALSE;
	}

	if (TaskPanelCtl != nullptr) {
		TaskPanelCtl->ReceiveSignal(pData);
	}
	else {
		CtlTaskBar& taskBar = TheApp.GetMainFrame().TaskBarCtl;
		Json::Object& uiData = TheAppResources.GetTask(TaskName);

		TaskPanelCtl = new VisualEffects3dPanel(&uiData);
		TaskPanelCtl->Initialize(&taskBar);
		TaskPanelCtl->SetDefaultData(data.GetAt(SKW_DEFAULTVALUE));
		TaskPanelCtl->SetData(data.GetAt(SKW_VALUE));

		taskBar.SetPanel(TaskPanelCtl);
		taskBar.Show(ViewWnd);

		REMOVE_POINTER(pData);
	}

	return true;
}



void CmdVisualEffects3d::Run(WndView* pView)
{
	__super::Run(pView);

	CtlTaskBar& taskBar = TheApp.GetMainFrame().TaskBarCtl;
	taskBar.SetParent((CWnd*)pView);
	theDelivery.taskBar.OnRequestValue(CommandId);
}

#undef theDelivery
