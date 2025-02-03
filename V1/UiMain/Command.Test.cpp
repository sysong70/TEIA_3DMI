#include "stdafx.h"
#include "Command.Test.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

void Command::Test8::Run(Window::View* pView)
{
	// TODO - load files

	// send Preference

	// send FileOption
}

//**************************************************************************************************

#pragma region Panel

class TestPanel : public Control::TaskPanel
{
public:

	TestPanel(Json::Object* pUiData)
		: TaskPanel(CUSTOM_3D_CMD_KEN_Test9, pUiData) {}



	~TestPanel() override {}

protected:

	void ConstructBody() override
	{
		if (m_propList.Initialize(this, WM_USER) == false) {
			DEBUG_RETURN;
		}

		m_propList.InitializeDesign(GetUiData().GetAt("properties"));
	}



	DECLARE_MESSAGE_MAP();
};




BEGIN_MESSAGE_MAP(TestPanel, CWnd)
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

#pragma endregion // REGION

//**************************************************************************************************

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

	m_pPanel = new TestPanel(&uiData);
	m_pPanel->Initialize(&taskBar);

	taskBar.SetPanel(m_pPanel);
	taskBar.Show(m_pView);
}
