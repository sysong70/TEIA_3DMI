#include "stdafx.h"
#include "Command.Test.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------------

void Command::Test8::Run(Window::View* pView)
{
	//: load files

	// send Preference

	// send FileOption
}

//--------------------------------------------------------------------------------------------------

#pragma region Panel

class TestPanel : public Control::TaskPanel
{
public:

	TestPanel(Json::Object* pUiData)
		: TaskPanel(CUSTOM_3D_CMD_KEN_Test9, pUiData) {}



	~TestPanel() override {}



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
		if (m_propList.Initialize(this, WM_USER) == false) {
			DEBUG_RETURN;
		}

		m_propList.InitializeDesign(GetUiData().GetAt("properties"));
	}

protected:

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




BEGIN_MESSAGE_MAP(TestPanel, CWnd)
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

#pragma endregion //:REGION

Command::Test9::Test9()
{
}



bool Command::Test9::ReceiveSignal(Json::Object* pData)
{
	DEBUG_VALID(m_pView);
	REMOVE_POINTER(pData);

	return true;
}



void Command::Test9::Run(Window::View* pView)
{
	__super::Run(pView);

	Component::TaskBar& taskBar = TheApplication.GetMainFrame().GetTaskBar();
	Json::Object& uiData = TheAppResources.GetTask("Test");

	TestPanel* pPanel = new TestPanel(&uiData);
	pPanel->Initialize(&taskBar);

	taskBar.SetPanel(pPanel);
	taskBar.Show(m_pView);
}



void Command::Test9::Cancel()
{
	__super::Cancel();

	TheApplication.GetMainFrame().GetTaskBar().Show(nullptr);
}
