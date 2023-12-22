#include <StdAfx.h>

#include "Kernel.DocView.h"
#include "./Private/Kernel.DocViewPrivate.h"

#include "../Dmi3dfInterface/3DF.Canvas.h"
#include "../Dmi3dfInterface/3DF.Factory.h"

#include "Signal.Connector.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

using namespace KERNEL;

KERNEL::DocView::DocView()
{
	m_pcImpl = new DocViewPrivate();
}

//== View 관련 함수 ==================================================================================

// 1. H3DF View Initialize 함수
void KERNEL::DocView::Initialize(Json::Object & cInObject, Signal::Delivery & cDelivery)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }
	pcImpl->SetDelivery(&cDelivery);

	H3DF::WindowHandle nWindowHandle = (H3DF::WindowHandle)cInObject.GetDwordPtr(SKW_HWND);
	
	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);

	H3DF::ApplicationWindowOptionsKit cOptions;

	pcImpl->m_cCanvas = H3DF::Factory::CreateCanvas(nWindowHandle, "3DMI_Canvas", cOptions);

	H3DF::View cView = H3DF::Factory::CreateView("3DMI_View");

	pcImpl->m_cCanvas.AttachViewAsLayout(cView);

	pcImpl->AllocationOperator(&pcImpl->m_cCanvas.GetFrontView(), cDelivery);

	//pcImpl->m_pcHighlightOSnapOperator = new KERNEL::Operator::HighlightObjectSnap(&pcImpl->m_cCanvas.GetFrontView().GetWindowKey());

	pcImpl->m_cCanvas.FileOpen(cInObject, cDelivery);
}

// 2. H3DF View Destruct 함수
void KERNEL::DocView::Destruct()
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->m_cCanvas.Destruct();

	//m_cView.Destruct();
}

// 3. H3DF View Paint 함수
void KERNEL::DocView::Paint(Json::Object & cInObject)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->m_cCanvas.Update(cInObject);
}

// 4. H3DF View Resize 함수
void KERNEL::DocView::Resize(Json::Object & cInObject)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
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
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	//m_cView.CancelCommands();
}


void KERNEL::DocView::ViewId(int nViewId)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->m_nViewId = nViewId;
}

int KERNEL::DocView::ViewId()
{
	DocViewPrivate * pcImpl = (DocViewPrivate *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->m_nViewId;
}

//== Mouse 관련 함수 =================================================================================

void KERNEL::DocView::MouseSignal(Json::Object & cInObject)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
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
	DocViewPrivate * pcImpl = static_cast<DocViewPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->m_cCanvas.GetFrontView().MouseMove(nFlag, x, y);

	if (!(MK_LBUTTON & nFlag) && !(MK_RBUTTON & nFlag)) {
		pcImpl->HighlightOSnapOperator().NoButtonDownAndMove(nFlag, x, y);
	}
	else if(MK_LBUTTON & nFlag) {
		pcImpl->HighlightOSnapOperator().LButtonDownAndMove(nFlag, x, y);
	}

/*
	else if (!(MK_LBUTTON & nFlag)) {
		//OnLButtonDownAndMove(HEventInfo & cInEvent)
		pcImpl->m_cCanvas.GetFrontView().LButtonDownAndMove(nFlag, x, y);
	}
*/
}

void KERNEL::DocView::LButtonDown(int nFlag, int x, int y)
{
	DocViewPrivate * pcImpl = static_cast<DocViewPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->m_cCanvas.GetFrontView().LButtonDown(nFlag, x, y);
}

void KERNEL::DocView::LButtonUp(int nFlag, int x, int y)
{
	DocViewPrivate * pcImpl = static_cast<DocViewPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	H3DF::ViewControl::Mode eMode = pcImpl->m_cCanvas.GetFrontView().GetViewControlMode();

	if (H3DF::ViewControl::Mode::ZoomBox == eMode) {
		pcImpl->m_cCanvas.GetFrontView().SetSuppressUpdate(true);
	}

	pcImpl->m_cCanvas.GetFrontView().LButtonUp(nFlag, x, y);

	if (H3DF::ViewControl::Mode::ZoomBox == eMode) {
		pcImpl->HighlightOSnapOperator().DrawSnapItems();
		pcImpl->m_cCanvas.GetFrontView().SetSuppressUpdate(false);
		pcImpl->m_cCanvas.GetFrontView().Update();
	}
}

void KERNEL::DocView::RButtonDown(int nFlag, int x, int y)
{
	DocViewPrivate * pcImpl = static_cast<DocViewPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->m_cCanvas.GetFrontView().RButtonDown(nFlag, x, y);
}

void KERNEL::DocView::RButtonUp(int nFlag, int x, int y)
{
	DocViewPrivate * pcImpl = static_cast<DocViewPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->m_cCanvas.GetFrontView().RButtonUp(nFlag, x, y);
}

void KERNEL::DocView::MouseWheel(int nFlag, int x, int y, Json::Object & cInObject)
{
	DocViewPrivate * pcImpl = static_cast<DocViewPrivate *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	int zDelta = cInObject.GetInteger(SKW_DELTA, -120);
	Json::Array & cArray = cInObject.GetArray(SKW_RECT);
	int nLeft = cArray[0]->ToInteger();
	int nTop = cArray[1]->ToInteger();

	pcImpl->m_cCanvas.GetFrontView().SetSuppressUpdate(true);

	pcImpl->m_cCanvas.GetFrontView().MouseWheel(nFlag, zDelta, x, y, nLeft, nTop);

	pcImpl->HighlightOSnapOperator().DrawSnapItems();

	pcImpl->m_cCanvas.GetFrontView().SetSuppressUpdate(false);

	pcImpl->m_cCanvas.GetFrontView().Update();
}

//== Keyboard 관련 함수 ==============================================================================

void KERNEL::DocView::KeyboardSignal(Json::Object & cInObject)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->m_cCanvas.KeyboardInput(cInObject);
}

//== View 관련 함수 ==========================================================================
void KERNEL::DocView::SetViewControl(int nId)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nId)
	{
		case HOME_3D_CMD_Pan:
			pcImpl->m_cCanvas.GetFrontView().SetPanViewControl();
			break;

		case HOME_3D_CMD_Zoom_Fit:
			pcImpl->m_cCanvas.GetFrontView().FitWorld();
			break;

		case HOME_3D_CMD_Zoom_Area:
			pcImpl->m_cCanvas.GetFrontView().SetZoomArea();
			break;

		case HOME_3D_CMD_Zoom_Object:
			break;

		case HOME_3D_CMD_Rotate_Rotate:
			pcImpl->m_cCanvas.GetFrontView().SetOrbitViewControl();
			break;

		case HOME_3D_CMD_Rotate_RotateCenter:
			break;

		case HOME_3D_CMD_Rotate_Turntable:
			pcImpl->m_cCanvas.GetFrontView().SetOrbitTurntableViewControl();
			break;

		case HOME_3D_CMD_Rotate_Orbit:
			pcImpl->m_cCanvas.GetFrontView().SetOrbitViewControl();
			break;
	}
}

//== Object Snap 관련 함수 ===================================================================
void KERNEL::DocView::SetObjectSnap(int nId)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	if(nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nId)
	{
		case HOME_3D_CMD_ObjectSnap_End:
			pcImpl->SetObjectSnap(OSnap::Type::EndPoint);
			break;

		case HOME_3D_CMD_ObjectSnap_Mid:
			pcImpl->SetObjectSnap(OSnap::Type::MidPoint);
			break;

		case HOME_3D_CMD_ObjectSnap_Intersection:
			pcImpl->SetObjectSnap(OSnap::Type::Intersection);
			break;

		case HOME_3D_CMD_ObjectSnap_Perpendicular:
			pcImpl->SetObjectSnap(OSnap::Type::Perpendicular);
			break;

		case HOME_3D_CMD_ObjectSnap_Center:
			pcImpl->SetObjectSnap(OSnap::Type::Center);
			break;

		case HOME_3D_CMD_ObjectSnap_Quadrant:
			pcImpl->SetObjectSnap(OSnap::Type::Quadrant);
			break;

		case HOME_3D_CMD_ObjectSnap_Near:
			pcImpl->SetObjectSnap(OSnap::Type::NearPoint);
			break;

		case HOME_3D_CMD_ObjectSnap_OnSurface:
			pcImpl->SetObjectSnap(OSnap::Type::OnSurface);
			break;

		case HOME_3D_CMD_ObjectSnap_BoundaryCenter:
			pcImpl->SetObjectSnap(OSnap::Type::BoundaryCenter);
			break;

		case HOME_3D_CMD_ObjectSnap_Axis:
			pcImpl->SetObjectSnap(OSnap::Type::Axis);
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
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nId)
	{
		case HOME_3D_CMD_SelectionFiter_Point:
			pcImpl->SetSelectionFilter(SelectionFilter::Type::Point);
			break;

		case HOME_3D_CMD_SelectionFiter_Curve:
			pcImpl->SetSelectionFilter(SelectionFilter::Type::Curve);
			break;

		case HOME_3D_CMD_SelectionFiter_Edge:
			pcImpl->SetSelectionFilter(SelectionFilter::Type::Edge);
			break;

		case HOME_3D_CMD_SelectionFiter_Face:
			pcImpl->SetSelectionFilter(SelectionFilter::Type::Face);
			break;

		case HOME_3D_CMD_SelectionFiter_Solid:
			pcImpl->SetSelectionFilter(SelectionFilter::Type::Solid);
			break;

		case HOME_3D_CMD_SelectionFiter_Axis:
			pcImpl->SetSelectionFilter(SelectionFilter::Type::Axis);
			break;

		case HOME_3D_CMD_SelectionFiter_PMI:
			pcImpl->SetSelectionFilter(SelectionFilter::Type::PMI);
			break;
	}

}


//== Style 관련 함수 =========================================================================
void KERNEL::DocView::SetViewStyle(int nStyleId)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nStyleId)
	{
		case HOME_3D_CMD_ViewStyle_Shade:
			pcImpl->m_cCanvas.GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::Phong);
			break;

		case HOME_3D_CMD_ViewStyle_ShadeWithEdges:
			pcImpl->m_cCanvas.GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::PhongWithLines);
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

void KERNEL::DocView::SetViewDirection(int nDirectionId)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
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
	DocViewPrivate * pcImpl = (DocViewPrivate *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->CommandRequest(cInObject);
}

// 1. Change Value 처리
void KERNEL::DocView::CommandChange(Json::Object & cInObject)
{
	DocViewPrivate * pcImpl = (DocViewPrivate *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->CommandChange(cInObject);
}