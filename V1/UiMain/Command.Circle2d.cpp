#include "stdafx.h"
#include "Command.Circle2d.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************
/*
#pragma region Panel

class Circle2dPanel : public Control::TaskPanel
{
public:

	Circle2dPanel(UINT subType, Json::Object* pUiData)
		: TaskPanel(subType, pUiData) {}

protected:

	DECLARE_MESSAGE_MAP();
};



BEGIN_MESSAGE_MAP(Circle2dPanel, CWnd)
	ON_WM_SIZE()
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

#pragma endregion // REGION
*/
//**************************************************************************************************
/*
#define RunCommand(ui, id) \
__super::Run(pView); \
\
pView->GetDelivery().view.OnCommand(id); \
Component::TaskBar& taskBar = TheApplication.GetMainFrame().GetTaskBar(); \
\
Json::Object& uiData = TheAppResources.GetTask(ui); \
m_pPanel = new Control::TaskPanel(id, &uiData); \
m_pPanel->Initialize(&taskBar); \
\
taskBar.SetPanel(m_pPanel); \
taskBar.Show(pView); \
*/
//**************************************************************************************************

bool Command::Circle2d::TwoPoints::ReceiveSignal(Json::Object* pData)
{
	return false;
}



void Command::Circle2d::TwoPoints::Run(Window::View* pView)
{
	RunCommand("Circle2d.2Points", DRAW_2D_CMD_Circle_2Points);
}

//**************************************************************************************************

bool Command::Circle2d::ThreePoints::ReceiveSignal(Json::Object* pData)
{
	return false;
}



void Command::Circle2d::ThreePoints::Run(Window::View* pView)
{
	RunCommand("Circle2d.3Points", DRAW_2D_CMD_Circle_3Points);
}

//**************************************************************************************************

bool Command::Circle2d::CenterRadius::ReceiveSignal(Json::Object* pData)
{
	return false;
}



void Command::Circle2d::CenterRadius::Run(Window::View* pView)
{
	RunCommand("Circle2d.CenterRadius", DRAW_2D_CMD_Circle_CenterRadius);
}

//**************************************************************************************************

bool Command::Circle2d::TwoTangentsRadius::ReceiveSignal(Json::Object* pData)
{
	return false;
}



void Command::Circle2d::TwoTangentsRadius::Run(Window::View* pView)
{
	RunCommand("Circle2d.2TangentsRadius", DRAW_2D_CMD_Circle_2TangentsRadius);
}
