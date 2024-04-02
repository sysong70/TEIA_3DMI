#include "StdAfx.h"

#include "Session.h"

#include "Manager.Session.h"

#include "../DmiKernel3dInterface/Kernel.DocView.h"

#include "../../UiMain/Command.Resource.h"

#define FILE_OPEN_TIMER_ID		100001

SESSION::Session::Session()
{
	m_pcDocView = new KERNEL::DocView();
}

SESSION::Session::~Session()
{
	if (nullptr != m_pcDocView) {
		delete m_pcDocView;
	}
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
	m_pcDocView->SetDelivery(cInstance);
	m_pcDocView->Initialize(cInObject);

	AfxBeginThread(ThreadFileOpen, this);

// 	HWND hWnd = (HWND)cInObject.GetDwordPtr(SKW_HWND);
// 	SetTimer(hWnd, FILE_OPEN_TIMER_ID, 100, OnTimerCallback);
}

void SESSION::Session::ViewPaint(Json::Object & cInObject)
{
	m_pcDocView->Paint(cInObject);
}

void SESSION::Session::ViewResize(Json::Object & cInObject)
{
	m_pcDocView->Resize(cInObject);
}

KERNEL::DocView * SESSION::Session::GetDocView()
{
	return m_pcDocView;
}

void CALLBACK SESSION::Session::OnTimerCallback(HWND hWnd, UINT nMsg, UINT_PTR nTimerId, DWORD dwTime)
{
	Session * pcSession = theSessionManager.GetSession(hWnd);
	if (nullptr == pcSession) {
		DEBUG_STOP;
		return;
	}

	if (FILE_OPEN_TIMER_ID == nTimerId) {
		KERNEL::DocView * pcDocView = pcSession->GetDocView();
		pcDocView->FileOpenTimer();
	}

	KillTimer(hWnd, nTimerId);
}

UINT SESSION::Session::ThreadFileOpen(LPVOID pcParam)
{
	Session * pcSession = (Session *)pcParam;
	KERNEL::DocView * pcDocView = pcSession->GetDocView();
	pcDocView->FileOpenTimer();

	return 0;
}

//== Mouse 관련 함수 =================================================================================

void SESSION::Session::ViewMouseSignal(Json::Object & cInObject)
{
	m_pcDocView->MouseSignal(cInObject);
}

//== Keyboard 관련 함수 ==============================================================================

void SESSION::Session::ViewKeyboardSignal(Json::Object & cInObject)
{
	m_pcDocView->KeyboardSignal(cInObject);
}

//== Command 관련 함수 ===============================================================================
void SESSION::Session::ViewExecuteCommand(Json::Object & cInObject)
{
	int nId = cInObject.GetInteger(SKW_ID);

	switch (nId)
	{ 
		case FILE_3D_CMD_Save:
		case FILE_3D_CMD_SaveAs: {
			CString strFilePath = cInObject.GetString(SKW_FILEPATH);
			m_pcDocView->Save(strFilePath.GetBuffer());
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
			m_pcDocView->SetViewStyle(nId);
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
			m_pcDocView->SetViewDirection(nId);
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
			m_pcDocView->SetViewControl(nId);
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
			m_pcDocView->SetObjectSnap(nId);
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
			m_pcDocView->SetSelectionFilter(nId);
			return;
			break;
	}

	switch (nId)
	{
		case HOME_3D_CMD_Visualize_ShowAll:
		case HOME_3D_CMD_Visualize_Hide:
		case HOME_3D_CMD_Visualize_ShowOnly:
		case HOME_3D_CMD_Visualize_Toggle:
			m_pcDocView->SetVisibility(nId);
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
			m_pcDocView->SetMeasure(nId);
			return;
			break;
	}
	switch (nId)
	{
		case CUSTOM_3D_CMD_SYSONG_Test1:
			m_pcDocView->TestCommand(nId);
			return;
			break;

	}

	DEBUG_STOP;
}

void SESSION::Session::ViewCancelCommands()
{
	m_pcDocView->CancelCommands();
}
//== Command 관련 함수 ===============================================================================

void SESSION::Session::CommandRequest(Json::Object & cInObject)
{
	m_pcDocView->CommandRequest(cInObject);
}

void SESSION::Session::CommandChange(Json::Object & cInObject)
{
	m_pcDocView->CommandChange(cInObject);
}

//== Panel 관련 함수 =================================================================================
void SESSION::Session::ModelPanelSignal(Json::Object & cInObject)
{
	m_pcDocView->ModelPanelSignal(cInObject);
}
