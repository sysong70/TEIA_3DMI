#include "StdAfx.h"

#include "Command.ManagerImpl.h"

#include "../Kernel.Session.h"
#include "Kernel.SessionImpl.h"

#include <3DF/Window.h>
#include <Sprocket/3DF.View.h>

#include "../../UiMain/Command.Resource.h"

using namespace KERNEL;


KERNEL::Session * KERNEL::Command::ManagerImpl::GetSession(int nId)
{
	return m_cApplication.GetDocView(nId);
}

//== View 관련 함수 ==================================================================================
void KERNEL::Command::ManagerImpl::ViewMouseSignal(Json::Object & cInObject, int nViewId)
{
	Session * pcDocView = GetSession(nViewId);
	DEBUG_VALID(pcDocView);

	pcDocView->MouseSignal(cInObject);
}

void KERNEL::Command::ManagerImpl::ViewKeyboardSignal(Json::Object & cInObject, int nViewId)
{
	Session * pcDocView = GetSession(nViewId);
	DEBUG_VALID(pcDocView);

	pcDocView->KeyboardSignal(cInObject);
}

void KERNEL::Command::ManagerImpl::ViewExecuteCommand(Json::Object & cInObject, int nViewId)
{
	Session * pcDocView = GetSession(nViewId);
	DEBUG_VALID(pcDocView);

	int nId = cInObject.GetInteger(SKW_ID);

	switch (nId)
	{
		case FILE_3D_CMD_Save:
		case FILE_3D_CMD_SaveAs: {
			CString strFilePath = cInObject.GetString(SKW_FILEPATH);
			pcDocView->Save(strFilePath.GetBuffer());
			strFilePath.ReleaseBuffer();
			return;
		} break;
	}

	switch (nId)
	{
		case HOME_3D_CMD_ViewStyle_Shade:
		case HOME_3D_CMD_ViewStyle_ShadeWithEdges:
		case HOME_3D_CMD_ViewStyle_Wireframe:
		case HOME_3D_CMD_ViewStyle_HiddenLineRemove:
		case HOME_3D_CMD_ViewStyle_Tessellated:
			pcDocView->SetViewStyle(nId);
			return;
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
			pcDocView->SetViewDirection(nId);
			return;
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
			pcDocView->SetViewControl(nId);
			return;
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
			pcDocView->SetObjectSnap(nId);
			return;
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
			pcDocView->SetSelectionFilter(nId);
			return;
			break;
	}

	switch (nId)
	{
		case HOME_3D_CMD_Visualize_ShowAll:
		case HOME_3D_CMD_Visualize_Hide:
		case HOME_3D_CMD_Visualize_ShowOnly:
		case HOME_3D_CMD_Visualize_Toggle:
			pcDocView->SetVisibility(nId);
			return;
			break;
	}

	switch (nId)
	{
		//case MEASURE_3D_CMD_Basic_Coordinate:
		case MEASURE_3D_CMD_Basic_Distance:
			//case MEASURE_3D_CMD_Basic_Length:
			//case MEASURE_3D_CMD_Basic_Radius:
			//case MEASURE_3D_CMD_Basic_Angle:
			pcDocView->SetMeasure(nId);
			return;
			break;
	}
	switch (nId)
	{
		case CUSTOM_3D_CMD_SYSONG_Test1:
			pcDocView->TestCommand(nId);
			return;
			break;

	}

	DEBUG_STOP;
}