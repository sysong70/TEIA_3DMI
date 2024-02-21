#include <StdAfx.h>

#include "Kernel.DocView.h"
#include "./Impl/Kernel.DocViewImpl.h"

#include <3DF.Canvas.h>
#include <3DF.Factory.h>

#include <3DF/Visibility.h>
#include <3DF/LineAttribute.h>
#include <3DF/AttributeLock.h>


#include "Signal.Connector.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

using namespace KERNEL;

KERNEL::DocView::DocView()
{
	m_pcImpl = new DocViewImpl();
}

//== View 관련 함수 ==================================================================================

// 1. H3DF View Initialize 함수
void KERNEL::DocView::Initialize(Json::Object & cInObject, Signal::Delivery & cDelivery)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }
	pcImpl->SetDelivery(&cDelivery);

	H3DF::WindowHandle nWindowHandle = (H3DF::WindowHandle)cInObject.GetDwordPtr(SKW_HWND);
	
	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);

	H3DF::ApplicationWindowOptionsKit cOptions;

	pcImpl->m_cCanvas = H3DF::Factory::CreateCanvas(nWindowHandle, "3DMI_Canvas", cOptions);

	H3DF::View cView = H3DF::Factory::CreateView("3DMI_View");

	pcImpl->m_cCanvas.AttachViewAsLayout(cView);

	pcImpl->AllocationOperator(this);

	pcImpl->m_cCanvas.FileOpen(cInObject, cDelivery);

	pcImpl->ModelPanel().Initialize(strFilePathName);
}

// 2. H3DF View Destruct 함수
void KERNEL::DocView::Destruct()
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->m_cCanvas.Destruct();

	//m_cView.Destruct();
}

// 3. H3DF View Paint 함수
void KERNEL::DocView::Paint(Json::Object & cInObject)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->m_cCanvas.Update(cInObject);
}

// 4. H3DF View Resize 함수
void KERNEL::DocView::Resize(Json::Object & cInObject)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	int nX = cInObject.GetInteger(SKW_X);
	int nY = cInObject.GetInteger(SKW_Y);

	pcImpl->m_cCanvas.Resize(nX, nY);
}

// 5. 명령어 취소
void KERNEL::DocView::CancelCommands()
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->CancelCommands();

	//m_cView.CancelCommands();
}


void KERNEL::DocView::ViewId(int nViewId)
{
	DocViewImpl * pcImpl = (DocViewImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->m_nViewId = nViewId;
}

int KERNEL::DocView::ViewId()
{
	DocViewImpl * pcImpl = (DocViewImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->m_nViewId;
}

//== Mouse 관련 함수 =================================================================================

void KERNEL::DocView::MouseSignal(Json::Object & cInObject)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	int nAction = cInObject.GetInteger(SKW_ACTION);
  	int nFlag = cInObject.GetInteger(SKW_FLAG);
	int x = cInObject.GetInteger(SKW_X);
	int y = cInObject.GetInteger(SKW_Y);

	switch ((Signal::View::Action)nAction)
	{
		case Signal::View::Action::OnMouseMove:
			MouseMove(nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonDown:
			LButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonUp:
			LButtonUp(nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonDown:
			//pcImpl->m_cCanvas.MButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonUp:
			//pcImpl->m_cCanvas.MButtonUp(nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonDown:
			RButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonUp:
			RButtonUp(nFlag, x, y);
			break;

		case Signal::View::Action::OnMouseWheel:
			MouseWheel(nFlag, x, y, cInObject);
			break;
	}
}

void KERNEL::DocView::MouseMove(int nFlag, int x, int y)
{
	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	if (200 > GetTickCount() - pcImpl->m_nMouseWhellStartTick) {
		return;
	}

	DWORD nNewFlags = pcImpl->MouseMapFlags(nFlag);
	HEventInfo cEvent((HBaseView *)pcImpl->GetBaseView());
	cEvent.SetPoint(HE_MouseMove, x, y, nNewFlags);

	pcImpl->Camera().MouseMove(cEvent);

	pcImpl->Select().MouseMove(cEvent);
}

void KERNEL::DocView::LButtonDown(int nFlag, int x, int y)
{
	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_cLButtonDownPosition.Set(x, y);

	HEventInfo cEvent((HBaseView *)pcImpl->GetBaseView());
	cEvent.SetPoint(HE_LButtonDown, x, y, pcImpl->MouseMapFlags(nFlag));

	pcImpl->Camera().LButtonDown(cEvent);

	pcImpl->Select().LButtonDown(cEvent);
}

void KERNEL::DocView::LButtonUp(int nFlag, int x, int y)
{
	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	H3DF::Point2D cLButtonUpPosition(x, y);

	pcImpl->m_cCanvas.GetFrontView().GetWindowKey().GetBaseView();

	// Camera 관련 처리
	H3DF::Camera::Mode eMode = pcImpl->Camera().CameraMode();

	if (H3DF::Camera::Mode::ZoomBox == eMode) {
		pcImpl->m_cCanvas.GetFrontView().SetSuppressUpdate(true);
	}

	HEventInfo cEvent((HBaseView *)pcImpl->GetBaseView());
	cEvent.SetPoint(HE_LButtonUp, x, y, pcImpl->MouseMapFlags(nFlag));

	// NavigationCube가 선택된 경우를 처리한다. NavigationCube가 선택되어 View를 변경한 경우에는 
	// HLISTENER_CONSUME_EVENT값을 리턴한다.
	if (HLISTENER_CONSUME_EVENT == pcImpl->Camera().LButtonUp(cEvent)) {
		return;
	}

	if (H3DF::Camera::Mode::ZoomBox == eMode) {
		pcImpl->Select().DrawSnapItems();
		pcImpl->m_cCanvas.GetFrontView().SetSuppressUpdate(false);
		pcImpl->m_cCanvas.GetFrontView().Update();
	}

	pcImpl->Select().LButtonUp(cEvent);
}

void KERNEL::DocView::RButtonDown(int nFlag, int x, int y)
{
	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	HEventInfo cEvent((HBaseView *)pcImpl->GetBaseView());
	cEvent.SetPoint(HE_RButtonDown, x, y, pcImpl->MouseMapFlags(nFlag));

	pcImpl->Camera().RButtonDown(cEvent);
}

void KERNEL::DocView::RButtonUp(int nFlag, int x, int y)
{
	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	HEventInfo cEvent((HBaseView *)pcImpl->GetBaseView());
	cEvent.SetPoint(HE_RButtonUp, x, y, pcImpl->MouseMapFlags(nFlag));

	pcImpl->Camera().RButtonUp(cEvent);
}

void KERNEL::DocView::MouseWheel(int nFlag, int x, int y, Json::Object & cInObject)
{
	DocViewImpl * pcImpl = dynamic_cast<DocViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->m_nMouseWhellStartTick = GetTickCount();

	int zDelta = cInObject.GetInteger(SKW_DELTA, -120);
	Json::Array & cArray = cInObject.GetArray(SKW_RECT);
	int nLeft = cArray[0]->ToInteger();
	int nTop = cArray[1]->ToInteger();

	pcImpl->m_cCanvas.GetFrontView().SetSuppressUpdate(true);

	// Control Flag을 추가해서 ComputeReasonableTarget이란 함수를 사용해서 Whell Zomm할때 Entity를 선택하는 과정을 생략함.
	// nFlag |= MK_CONTROL;

	HEventInfo	cEvent((HBaseView *)pcImpl->GetBaseView());
	cEvent.SetPoint(HE_MouseWheel, x - nLeft, y - nTop, pcImpl->MouseMapFlags(nFlag));
	cEvent.SetMouseWheelDelta(zDelta);

	pcImpl->Camera().MouseWheel(cEvent);

	pcImpl->Select().DrawSnapItems();

	pcImpl->m_cCanvas.GetFrontView().SetSuppressUpdate(false);

	pcImpl->m_cCanvas.GetFrontView().Update();
}

//== Keyboard 관련 함수 ==============================================================================

void KERNEL::DocView::KeyboardSignal(Json::Object & cInObject)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->m_cCanvas.KeyboardInput(cInObject);
}

//== View 관련 함수 ==========================================================================
void KERNEL::DocView::SetViewControl(int nId)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nId)
	{
		case HOME_3D_CMD_Pan:
			pcImpl->Camera().SetPanViewControl();
			break;

		case HOME_3D_CMD_Zoom_Fit:
			pcImpl->Camera().FitWorld();
			break;

		case HOME_3D_CMD_Zoom_Area:
			pcImpl->Camera().SetZoomArea();
			break;

		case HOME_3D_CMD_Zoom_Object:
			break;

		case HOME_3D_CMD_Rotate_Rotate:
			pcImpl->Camera().SetOrbitViewControl();
			break;

		case HOME_3D_CMD_Rotate_RotateCenter:
			break;

		case HOME_3D_CMD_Rotate_Turntable:
			pcImpl->Camera().SetOrbitTurntableViewControl();
			break;

		case HOME_3D_CMD_Rotate_Orbit:
			pcImpl->Camera().SetOrbitViewControl();
			break;
	}
}

//== Object Snap 관련 함수 ===================================================================
void KERNEL::DocView::SetObjectSnap(int nId)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if(nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nId)
	{
		case HOME_3D_CMD_ObjectSnap_End:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::EndPoint);
			break;

		case HOME_3D_CMD_ObjectSnap_Mid:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::MidPoint);
			break;

		case HOME_3D_CMD_ObjectSnap_Intersection:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::Intersection);
			break;

		case HOME_3D_CMD_ObjectSnap_Perpendicular:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::Perpendicular);
			break;

		case HOME_3D_CMD_ObjectSnap_Center:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::Center);
			break;

		case HOME_3D_CMD_ObjectSnap_Quadrant:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::Quadrant);
			break;

		case HOME_3D_CMD_ObjectSnap_Near:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::NearPoint);
			break;

		case HOME_3D_CMD_ObjectSnap_OnSurface:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::OnSurface);
			break;

		case HOME_3D_CMD_ObjectSnap_BoundaryCenter:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::BoundaryCenter);
			break;

		case HOME_3D_CMD_ObjectSnap_Axis:
			pcImpl->Select().SetObjectSnapMode(OSnap::Type::Axis);
			break;

		case HOME_3D_CMD_ObjectSnap_Absolute:
		case HOME_3D_CMD_ObjectSnap_Relative:
		case HOME_3D_CMD_ObjectSnap_ExpandLine:
			break;
	}
}

//== Selection Fiter 관련 함수 ===============================================================
void KERNEL::DocView::SetSelectionFilter(int nId)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nId)
	{
		case HOME_3D_CMD_SelectionFiter_Point:
			pcImpl->Select().SetSelectionFilter(SelectionFilter::Type::Point);
			break;

		case HOME_3D_CMD_SelectionFiter_Curve:
			pcImpl->Select().SetSelectionFilter(SelectionFilter::Type::Curve);
			break;

		case HOME_3D_CMD_SelectionFiter_Edge:
			pcImpl->Select().SetSelectionFilter(SelectionFilter::Type::Edge);
			break;

		case HOME_3D_CMD_SelectionFiter_Face:
			pcImpl->Select().SetSelectionFilter(SelectionFilter::Type::Face);
			break;

		case HOME_3D_CMD_SelectionFiter_Solid:
			pcImpl->Select().SetSelectionFilter(SelectionFilter::Type::Solid);
			break;

		case HOME_3D_CMD_SelectionFiter_Axis:
			pcImpl->Select().SetSelectionFilter(SelectionFilter::Type::Axis);
			break;

		case HOME_3D_CMD_SelectionFiter_PMI:
			pcImpl->Select().SetSelectionFilter(SelectionFilter::Type::PMI);
			break;
	}

}


//== Style 관련 함수 =========================================================================
void KERNEL::DocView::SetViewStyle(int nStyleId)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nStyleId)
	{
		case HOME_3D_CMD_ViewStyle_Shade:
			pcImpl->m_cCanvas.GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::Gouraud);
			break;

		case HOME_3D_CMD_ViewStyle_ShadeWithEdges:
			pcImpl->m_cCanvas.GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::GouraudWithLines);
			break;

		case HOME_3D_CMD_ViewStyle_Wireframe:
			pcImpl->m_cCanvas.GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::Wireframe);
			break;

		case HOME_3D_CMD_ViewStyle_HiddenLineRemove:
			pcImpl->m_cCanvas.GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::HiddenLine);
			break;

		case HOME_3D_CMD_ViewStyle_Tessellated:
			pcImpl->m_cCanvas.GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::Tessellated);
			break;

		default:
			assert(false);
			break;
	}
}

//== Visibility 관련 함수 ============================================================================
void KERNEL::DocView::SetVisibility(int nId)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->SetVisibility(nId);
}


void KERNEL::DocView::SetViewDirection(int nDirectionId)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nDirectionId)
	{
		case HOME_3D_CMD_ViewDirection_Top:
			pcImpl->m_cCanvas.GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::top);
			break;

		case HOME_3D_CMD_ViewDirection_Front:
			pcImpl->m_cCanvas.GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::front);
			break;

		case HOME_3D_CMD_ViewDirection_Left:
			pcImpl->m_cCanvas.GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::left);
			break;

		case HOME_3D_CMD_ViewDirection_Bottom:
			pcImpl->m_cCanvas.GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::bottom);
			break;

		case HOME_3D_CMD_ViewDirection_Back:
			pcImpl->m_cCanvas.GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::back);
			break;

		case HOME_3D_CMD_ViewDirection_Right:
			pcImpl->m_cCanvas.GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::right);
			break;

		case HOME_3D_CMD_ViewDirection_Iso:
			pcImpl->m_cCanvas.GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::px_py_pz);
			break;

		case HOME_3D_CMD_ViewDirection_SeIso:
			pcImpl->m_cCanvas.GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::nx_py_pz);
			break;

		case HOME_3D_CMD_ViewDirection_Perspective:
			assert(false);
			break;

	}
}

//== Command 관련 함수 ===============================================================================

// 1. Request Value 처리
void KERNEL::DocView::CommandRequest(Json::Object & cInObject)
{
	DocViewImpl * pcImpl = (DocViewImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->CommandRequest(cInObject);
}

// 1. Change Value 처리
void KERNEL::DocView::CommandChange(Json::Object & cInObject)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->CommandChange(cInObject);
}

//== Panel 관련 함수 =================================================================================
void KERNEL::DocView::ModelPanelSignal(Json::Object & cInObject)
{
	DocViewImpl * pcImpl = (DocViewImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	// Delivery는 ModelPanel 선언할 때 이미 할당함.
	pcImpl->ModelPanel().Signal(cInObject);
}
