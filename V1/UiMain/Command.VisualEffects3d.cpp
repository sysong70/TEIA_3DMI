#include "stdafx.h"
#include "Command.VisualEffects3d.h"
#include "Command.Resource.h"
#include "Component.TaskBar.h"
#include "Control.TaskPanel.h"
#include "Facility.AppResources.h"
#include "Facility.CommandIndexer.h"
#include "Window.Application.h"
#include "Window.MainFrame.h"
#include "Signal.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



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

#pragma region Panel

class VisualEffects3dPanel : public Control::TaskPanel
{
public:

	VisualEffects3dPanel(Json::Object* pUiData)
		: TaskPanel(PRESET::CommandId, pUiData)
	{}

~VisualEffects3dPanel() override
{}

void SetData(Json::Object& data) override
{
	__super::SetData(data);

	m_bInitialized = false;
	m_propList.InitializeData(m_data);
	m_bInitialized = true;
}

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
		m_toolBar.IsCheckButton(true);
		m_toolBar.Initialize(this, PRESET::ToolBar);

		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_Shade);
		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_ShadeWithEdges);
		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_Wireframe);
		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_HiddenLineRemove);
		m_toolBar.AddButton(HOME_3D_CMD_ViewStyle_Tessellated);

		m_toolBar.SetCheck(TheActiveCommand.Home.VisualEffects3d, true);
	}

protected:

	afx_msg void OnCommand(UINT id)
	{
		m_toolBar.SetCheck(id, true);
		TheActiveCommand.Home.VisualEffects3d = id;

		Component::TaskBar* taskBar = (Component::TaskBar*)GetParent();
		taskBar->GetDelivery().view.OnCommand(id);

		switch (id) {
		case HOME_3D_CMD_ViewStyle_Shade:
		case HOME_3D_CMD_ViewStyle_ShadeWithEdges:
		case HOME_3D_CMD_ViewStyle_Tessellated:
			m_propList.Enable(true);
			break;

		default:
			m_propList.Enable(false);
			break;
		}
	}

	afx_msg void OnSize(UINT nType, int cx, int cy)
	{
		if (cx == 0 || cy == 0) {
			return;
		}

		int margin = Control::Gap().cy;
		CPoint propTop;
		CSize propSize;

		if (m_toolBar.GetSafeHwnd() != nullptr) {
			m_toolBar.AdjustLayout();
			CSize toolBarSize = Control::GetSize(&m_toolBar);

			propTop = { 0, toolBarSize.cy + margin };
			propSize = { cx, cy - toolBarSize.cy - margin };
		}
		else {
			propSize = { cx, cy };
		}

		m_propList.SetWindowPos(NULL, propTop.x, propTop.y, propSize.cx, propSize.cy, SWP_NOACTIVATE);
	}

	DECLARE_MESSAGE_MAP();
};



BEGIN_MESSAGE_MAP(VisualEffects3dPanel, CWnd)
	ON_WM_SIZE()
	ON_COMMAND_RANGE(HOME_3D_CMD_ViewStyle_Shade, HOME_3D_CMD_ViewStyle_Tessellated, OnCommand)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

#pragma endregion //:REGION

Command::VisualEffects3d::VisualEffects3d()
{
}



bool Command::VisualEffects3d::ReceiveSignal(Json::Object* pData)
{
	DEBUG_VALID(m_pView);

	Json::Object& data = *pData;
	if (data.GetInteger(SKW_ID) != PRESET::CommandId) {
		REMOVE_POINTER(pData);
		RETURN_FALSE;
	}

	Component::TaskBar& taskBar = TheApplication.GetMainFrame().GetTaskBar();
	Json::Object& uiData = TheAppResources.GetTask(PRESET::TaskName);

	VisualEffects3dPanel* pPanel = new VisualEffects3dPanel(&uiData);
	pPanel->Initialize(&taskBar);
	pPanel->SetDefaultData(data.GetAt(SKW_VALUE));
	pPanel->SetData(data.GetAt(SKW_DEFAULTVALUE));

	taskBar.SetPanel(pPanel);
	taskBar.Show(m_pView);

	REMOVE_POINTER(pData);

	return true;
}



void Command::VisualEffects3d::Run(Window::View* pView)
{
	__super::Run(pView);

	Component::TaskBar& taskBar = TheApplication.GetMainFrame().GetTaskBar();
	taskBar.SetParent((CWnd*)pView);
	taskBar.GetDelivery().taskBar.OnRequestValue(PRESET::CommandId);
}



void Command::VisualEffects3d::Cancel()
{
	__super::Cancel();

	TheApplication.GetMainFrame().GetTaskBar().Show(nullptr);
}

#undef PRESET
