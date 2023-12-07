#include <StdAfx.h>

#include "Session.h"

#include "../DmiKernel3dInterface/Kernel.DocView.h"

#include "../../UiMain/Command.Resource.h"

SESSION::Session::Session()
{
	m_pcDocView = new KERNEL::DocView();
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
	if (nullptr != m_pcDocView) {
		m_pcDocView->ViewId(nSessionId);
	}
}

//== View 관련 함수 ==================================================================================

void SESSION::Session::ViewInitialize(Json::Object & cInObject, Signal::Delivery & cInstance)
{
	m_pcDocView->Initialize(cInObject, cInstance);
}

void SESSION::Session::ViewDestruct()
{
	m_pcDocView->Destruct();
}

void SESSION::Session::ViewPaint(Json::Object & cInObject)
{
	m_pcDocView->Paint(cInObject);
}

void SESSION::Session::ViewResize(Json::Object & cInObject)
{
	m_pcDocView->Resize(cInObject);
}

KERNEL::DocView * SESSION::Session::GetView()
{
	return m_pcDocView;
}

//== Mouse 관련 함수 =================================================================================

void SESSION::Session::MouseSignal(Json::Object & cInObject)
{
	m_pcDocView->MouseSignal(cInObject);
}

//== Keyboard 관련 함수 ==============================================================================

void SESSION::Session::KeyboardSignal(Json::Object & cInObject)
{
	m_pcDocView->KeyboardSignal(cInObject);
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
			m_pcDocView->SetViewStyle(nId);
			break;
	}

	switch (nId)
	{
		case HOME_3D_CMD_ViewDirection_Top:
		case HOME_3D_CMD_ViewDirection_Front:
		case HOME_3D_CMD_ViewDirection_Left:
		case HOME_3D_CMD_ViewDirection_Bottom:
		case HOME_3D_CMD_ViewDirection_Back:
		case HOME_3D_CMD_ViewDirection_Right:
		case HOME_3D_CMD_ViewDirection_Iso:
		case HOME_3D_CMD_ViewDirection_SeIso:
		case HOME_3D_CMD_ViewDirection_Perspective:
			m_pcDocView->SetViewDirection(nId);
			break;
	}

	switch (nId)
	{
		case HOME_3D_CMD_VisualEffects_Shadow:
		case HOME_3D_CMD_VisualEffects_Reflection:
		case HOME_3D_CMD_VisualEffects_AmbientOcclusion:
		case HOME_3D_CMD_VisualEffects_SilhouetteEdges:
		case HOME_3D_CMD_VisualEffects_Bloom:
			m_pcDocView->SetVisualEffects(nId);
			break;
	}

	switch (nId)
	{
		case HOME_3D_CMD_Pan:
		case HOME_3D_CMD_Zoom_Fit:
		case HOME_3D_CMD_Zoom_Area:
		case HOME_3D_CMD_Zoom_Object:
		case HOME_3D_CMD_Rotate_Rotate:
		case HOME_3D_CMD_Rotate_RotateCenter:
		case HOME_3D_CMD_Rotate_Turntable:
		case HOME_3D_CMD_Rotate_Orbit:
			m_pcDocView->SetViewControl(nId);
			break;
	}

	// Object Snap 설정
	switch (nId) 
	{
		case HOME_3D_CMD_ObjectSnap_End:
		case HOME_3D_CMD_ObjectSnap_Mid:
		case HOME_3D_CMD_ObjectSnap_Intersection:
		case HOME_3D_CMD_ObjectSnap_Perpendicular:
		case HOME_3D_CMD_ObjectSnap_Center:
		case HOME_3D_CMD_ObjectSnap_Quadrant:
		case HOME_3D_CMD_ObjectSnap_Near:
		case HOME_3D_CMD_ObjectSnap_OnSurface:
		case HOME_3D_CMD_ObjectSnap_BoundaryCenter:
		case HOME_3D_CMD_ObjectSnap_Axis:
		case HOME_3D_CMD_ObjectSnap_Absolute:
		case HOME_3D_CMD_ObjectSnap_Relative:
		case HOME_3D_CMD_ObjectSnap_ExpandLine:
			m_pcDocView->SetObjectSnap(nId);
			break;
	}

	// Selection Filter 설정
	switch (nId)
	{
		case HOME_3D_CMD_SelectionFiter_Point:
		case HOME_3D_CMD_SelectionFiter_Curve:
		case HOME_3D_CMD_SelectionFiter_Edge:
		case HOME_3D_CMD_SelectionFiter_Face:
		case HOME_3D_CMD_SelectionFiter_Solid:
		case HOME_3D_CMD_SelectionFiter_Axis:
		case HOME_3D_CMD_SelectionFiter_PMI:
			m_pcDocView->SetSelectionFilter(nId);
			break;
	}
}

void SESSION::Session::CancelCommands()
{
	m_pcDocView->CancelCommands();
}