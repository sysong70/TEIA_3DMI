#include "stdafx.h"
#include "Command.VisualEffects3d.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define PRESET PresetVisualEffects3d

namespace PresetVisualEffects3d
{
	const UINT CommandId = HOME_3D_LST_VisualEffects;
	const CStringA TaskName = "VisualEffects3d";

	enum EControlId
	{
		ToolBar = WM_USER + 1,
		PropList,
	};
}

//**************************************************************************************************

#pragma region Panel

class VisualEffects3dPanel : public Control::TaskPanel
{
public:

	VisualEffects3dPanel(Json::Object* pUiData)
		: TaskPanel(PRESET::CommandId, pUiData) {}

protected:

	void ConstructBody() override
	{
		if (m_propList.Initialize(this, PRESET::PropList) == false) {
			DEBUG_RETURN;
		}

		m_propList.InitializeDesign(GetUiData().GetAt("properties"));
	}



	void ConstructHeader() override
	{
		m_toolBar.SetPivot(Control::EPivot::TopLeft);
		m_toolBar.Initialize(this, PRESET::ToolBar);

		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_Shade);
		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_ShadeWithEdges);
		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_Wireframe);
		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_HiddenLineRemove);
		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_Tessellated);

		m_toolBar.SetCheck(TheActiveCommand.Home.VisualEffects3d, true);

		//m_toolBar.GetButton(HOME_3D_CMD_ViewStyle_Wireframe)->EnableWindow(FALSE);
		//m_toolBar.GetButton(HOME_3D_CMD_ViewStyle_Tessellated)->EnableWindow(FALSE);
	}

protected:

	afx_msg void OnCommand(UINT id)
	{
		m_toolBar.SetCheck(id, true);
		TheActiveCommand.Home.VisualEffects3d = id;

		Component::TaskBar* taskBar = (Component::TaskBar*)GetParent();
		taskBar->GetDelivery().view.OnCommand(id);

		m_propList.SetCurSel(NULL);

		if (id == HOME_3D_CMD_ViewStyle_HiddenLineRemove) {
			m_propList.Show(L"Shadow", false);
			m_propList.Show(L"PlaneReflection", false);
			m_propList.Show(L"AmbientOcclusion", true);
			m_propList.Show(L"SilhouetteEdges", true);
			m_propList.Show(L"Bloom", false);

			m_propList.ExpandAll(true);
			m_propList.EnableWindow(TRUE);
		}
		else if (id == HOME_3D_CMD_ViewStyle_Wireframe) {
			m_propList.ShowAll(true);
			m_propList.ExpandAll(false);
			m_propList.EnableWindow(FALSE);
		}
		else {
			m_propList.ShowAll(true);
			m_propList.ExpandAll(true);
			m_propList.EnableWindow(TRUE);
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

bool Command::VisualEffects3d::ReceiveSignal(Json::Object* pData)
{
	DEBUG_VALID(m_pView);

	Json::Object& data = *pData;
	if (data.GetInteger(SKW_ID) != PRESET::CommandId) {
		REMOVE_POINTER(pData);
		RETURN_FALSE;
	}

	if (m_pPanel != nullptr) {
		m_pPanel->ReceiveSignal(pData);
	}
	else {
		Component::TaskBar& taskBar = TheApplication.GetMainFrame().GetTaskBar();
		Json::Object& uiData = TheAppResources.GetTask(PRESET::TaskName);

		m_pPanel = new VisualEffects3dPanel(&uiData);
		m_pPanel->Initialize(&taskBar);
		m_pPanel->SetDefaultData(data.GetAt(SKW_DEFAULTVALUE));
		m_pPanel->SetData(data.GetAt(SKW_VALUE));

		taskBar.SetPanel(m_pPanel);
		taskBar.Show(m_pView);

		REMOVE_POINTER(pData);
	}

	return true;
}



void Command::VisualEffects3d::Run(Window::View* pView)
{
	__super::Run(pView);

	Component::TaskBar& taskBar = TheApplication.GetMainFrame().GetTaskBar();
	taskBar.SetParent((CWnd*)pView);
	taskBar.GetDelivery().taskBar.OnRequestValue(PRESET::CommandId);
}

#undef PRESET
