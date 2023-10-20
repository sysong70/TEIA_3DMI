#include <StdAfx.h>

#include "Session.h"

#include "../DmiKernel3dInterface/Kernel.View.h"

#include "../../UiMain/Command.Resource.h"

SESSION::Session::Session()
{
	m_pcView = new KERNEL::View();
}

SESSION::Session::~Session()
{

}
int SESSION::Session::SessionId() const
{
	return m_nSessionId;
}

void SESSION::Session::SessionId(int nSessionId)
{
	m_nSessionId = nSessionId;
	if (nullptr != m_pcView) {
		m_pcView->ViewId(nSessionId);
	}
}

//== Canvas 관련 함수 ================================================================================
void SESSION::Session::CanvasInitialize(Json::Object & cInObject, Signal::Delivery & cInstance)
{

}

//== View 관련 함수 ==================================================================================

void SESSION::Session::ViewInitialize(Json::Object & cInObject, Signal::Delivery & cInstance)
{
	m_pcView->Initialize(cInObject, cInstance);
}

void SESSION::Session::ViewDestruct()
{
	m_pcView->Destruct();
}

void SESSION::Session::ViewPaint(Json::Object & cInObject)
{
	m_pcView->Paint(cInObject);
}

void SESSION::Session::ViewResize(Json::Object & cInObject)
{
	m_pcView->Resize(cInObject);
}

KERNEL::View * SESSION::Session::GetView()
{
	return m_pcView;
}

//== Mouse 관련 함수 =================================================================================

void SESSION::Session::MouseSignal(Json::Object & cInObject)
{
	m_pcView->MouseSignal(cInObject);
}

//== Keyboard 관련 함수 ==============================================================================

void SESSION::Session::KeyboardSignal(Json::Object & cInObject)
{
	m_pcView->KeyboardSignal(cInObject);
}

//== Command 관련 함수 ===============================================================================
void SESSION::Session::ExecuteCommand(Json::Object & cInObject)
{
	CString strText;
	cInObject.Stringify(strText);

	int nId = cInObject.GetInteger(SKW_ID);

	switch (nId)
	{
		case HOME_3D_CMD_ViewStyle_Shade:
		case HOME_3D_CMD_ViewStyle_ShadeWithEdges:
		case HOME_3D_CMD_ViewStyle_Wireframe:
		case HOME_3D_CMD_ViewStyle_HiddenLineRemove:
		case HOME_3D_CMD_ViewStyle_Tessellated:
			break;
	}
}

void SESSION::Session::CancelCommands()
{
	m_pcView->CancelCommands();
}