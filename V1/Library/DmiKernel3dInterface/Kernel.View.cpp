#include <StdAfx.h>

#include "Kernel.View.h"
#include "./Private/Kernel.ViewPrivate.h"

#include "../Dmi3dfInterface/3DF.Canvas.h"
#include "../Dmi3dfInterface/3DF.Factory.h"

#include "Signal.Connector.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

using namespace KERNEL;

KERNEL::View::View()
{
	m_pcImpl = new ViewPrivate();
}

//== View 관련 함수 ==================================================================================

// 1. H3DF View Initialize 함수
void KERNEL::View::Initialize(Json::Object & cInObject, Signal::Delivery & cInstance)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	H3DF::WindowHandle nWindowHandle = (H3DF::WindowHandle)cInObject.GetDwordPtr(SKW_HWND);
	
	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);

	H3DF::ApplicationWindowOptionsKit cOptions;

	pcImpl->m_cCanvas = H3DF::Factory::CreateCanvas(nWindowHandle, "3DMI_Canvas", cOptions);

	H3DF::View cView = H3DF::Factory::CreateView("3DMI_View");

	pcImpl->m_cCanvas.AttachViewAsLayout(cView);

	pcImpl->m_cCanvas.FileOpen(cInObject, cInstance);
	
	cInstance.view.SetValidation();
}

// 2. H3DF View Destruct 함수
void KERNEL::View::Destruct()
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->m_cCanvas.Destruct();

	//m_cView.Destruct();
}

// 3. H3DF View Paint 함수
void KERNEL::View::Paint(Json::Object & cInObject)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->m_cCanvas.Update(cInObject);
}

// 4. H3DF View Resize 함수
void KERNEL::View::Resize(Json::Object & cInObject)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	int nX = cInObject.GetInteger(SKW_X);
	int nY = cInObject.GetInteger(SKW_Y);

	pcImpl->m_cCanvas.Resize(nX, nY);
}

// 5. 명령어 취소
void KERNEL::View::CancelCommands()
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	//m_cView.CancelCommands();
}


void KERNEL::View::ViewId(int nViewId)
{
	m_nViewId = nViewId;
}

int KERNEL::View::ViewId()
{
	return m_nViewId;
}

//== Mouse 관련 함수 =================================================================================

void KERNEL::View::MouseSignal(Json::Object & cInObject)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
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
			pcImpl->m_cCanvas.MouseMove(nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonDown:
			pcImpl->m_cCanvas.LButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnLButtonUp:
			pcImpl->m_cCanvas.LButtonUp(nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonDown:
			//pcImpl->m_cCanvas.MButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonUp:
			//pcImpl->m_cCanvas.MButtonUp(nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonDown:
			pcImpl->m_cCanvas.RButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnRButtonUp:
			pcImpl->m_cCanvas.RButtonUp(nFlag, x, y);
			break;

		case Signal::View::Action::OnMouseWheel: {
			int zDelta = cInObject.GetInteger(SKW_DELTA, -120);
			Json::Array & cArray = cInObject.GetArray(SKW_RECT);
			int nLeft = cArray[0]->ToInteger();
			int nTop = cArray[1]->ToInteger();
			pcImpl->m_cCanvas.MouseWheel(nFlag, zDelta, x, y, nLeft, nTop);
		} break;
	}
}

//== Keyboard 관련 함수 ==============================================================================

void KERNEL::View::KeyboardSignal(Json::Object & cInObject)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->m_cCanvas.KeyboardInput(cInObject);
}

//== Style 관련 함수 =========================================================================
void KERNEL::View::SetViewStyle(int nStyleId)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
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

void KERNEL::View::SetViewDirection(int nDirectionId)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
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

//== Visual Effects 관련 함수 ========================================================================

// 1. Visual Effects 설정
void KERNEL::View::SetVisualEffects(int nEffectId)
{
	ViewPrivate * pcImpl = (ViewPrivate *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nEffectId)
	{
		case HOME_3D_CMD_VisualEffects_Shadow:
			pcImpl->SetVisualEffectsShadow();
			break;

		case HOME_3D_CMD_VisualEffects_Reflection:
			pcImpl->SetVisualEffectsReflection();
			break;

		case HOME_3D_CMD_VisualEffects_AmbientOcclusion:
			pcImpl->SetVisualEffectsAmbientOcclusion();
			break;

		case HOME_3D_CMD_VisualEffects_SilhouetteEdges:
			pcImpl->SetVisualEffectsSilhouetteEdges();
			break;

		case HOME_3D_CMD_VisualEffects_Bloom:
			pcImpl->SetVisualEffectsBloom();
			break;
	}
}