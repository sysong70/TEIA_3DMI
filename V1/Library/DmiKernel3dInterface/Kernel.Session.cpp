#include <StdAfx.h>

#include "Kernel.Session.h"
#include "./Impl/Kernel.Session.Impl.h"

#include <Sprocket/3DF.Canvas.h>
#include <Sprocket/3DF.Factory.h>

#include <3DF/Visibility.h>
#include <3DF/LineAttribute.h>
#include <3DF/AttributeLock.h>
#include <3DF/Image.h>
#include <3DF/Texture.h>
#include <3DF/Portfolio.h>
#include <3DF/3DF.Utility.h>

#include "Signal.Connector.h"

#include "../Signal/Signal.h"
#include "../Common/Common_Define.h"

#include "../../UiMain/Command.Resource.h"

using namespace KERNEL;

KERNEL::Session::Session()
{
	m_pcImpl = new SessionImpl();
}

void KERNEL::Session::SetDelivery(Signal::Delivery & cDelivery)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->SetDelivery(&cDelivery);
}

//== View 관련 함수 ==================================================================================

// 1. H3DF View Initialize 함수
void KERNEL::Session::Initialize(Json::Object & cInObject)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	H3DF::WindowHandle nWindowHandle = (H3DF::WindowHandle)cInObject.GetDwordPtr(SKW_HWND);
	// KEN - 20240229
	ASSERT(::IsWindow((HWND)nWindowHandle) == TRUE);
	
	H3DF::ApplicationWindowOptionsKit cOptions;

	pcImpl->m_pcCanvas = H3DF::Factory::CreateCanvas(nWindowHandle, "3DMI_Canvas", cOptions);

	H3DF::View * pcView = H3DF::Factory::CreateView("3DMI_View");

	pcImpl->m_pcCanvas->AttachViewAsLayout(pcView);

	pcImpl->AllocationOperator(this);

	pcImpl->m_pcCanvas->SetDelivery(pcImpl->Delivery(), pcImpl->m_nViewId);

	// Timer를 이용해서 File을 Open하기 위해서, File Path Name을 저장한다.
	CString strFilePathName = cInObject.GetString(SKW_FILEPATH);
	if (true != strFilePathName.IsEmpty()) {
		pcImpl->m_strFilePathName = strFilePathName;
	}
}

void KERNEL::Session::ThreadFileOpen()
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->m_pcCanvas->FileOpen(pcImpl->m_strFilePathName, pcImpl->CADModel());

	// #ModelPanel: File Open한 후에, CADModel을 이용해서 ModelPanel을 초기화한다.
	pcImpl->ModelPanel().Initialize(pcImpl->CADModel());
}

// 3. H3DF View Paint 함수
void KERNEL::Session::Paint(Json::Object & cInObject)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->GetCanvas().Update(cInObject);
}

// 4. H3DF View Resize 함수
void KERNEL::Session::Resize(Json::Object & cInObject)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	int nX = cInObject.GetInteger(SKW_X);
	int nY = cInObject.GetInteger(SKW_Y);

	pcImpl->GetCanvas().Resize(nX, nY);
}

// 5. 명령어 취소
void KERNEL::Session::CancelCommands()
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	pcImpl->CancelCommands();

	//m_cView.CancelCommands();
}

void KERNEL::Session::ViewId(int nViewId)
{
	SessionImpl * pcImpl = (SessionImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	pcImpl->m_nViewId = nViewId;
}

int KERNEL::Session::ViewId()
{
	SessionImpl * pcImpl = (SessionImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);
	return pcImpl->m_nViewId;
}

HWND KERNEL::Session::GetHwnd()
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas().GetHwnd();
}

H3DF::Canvas & KERNEL::Session::Canvas() const
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->GetCanvas();
}

H3DF::CADModel & KERNEL::Session::CADModel() const
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->CADModel();
}

bool KERNEL::Session::Save(WCHAR * pstrInFilePathName)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;

	CString strFilePathName = pstrInFilePathName;

	if (true == strFilePathName.IsEmpty()) {
		strFilePathName = L"Z://Test.hsf";
	}
	
	pcImpl->GetCanvas().GetFrontView().SaveHsfFile(strFilePathName, &pcImpl->GetCanvas());

	return true;
}

//== Operator 관련 함수 ======================================================================
Command::Attribute & KERNEL::Session::Attribute()
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->Attribute();
}

Command::Camera & KERNEL::Session::Camera()
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->Camera();
}

Command::Select & KERNEL::Session::Select()
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->Select();
}

Command::ModelPanel & KERNEL::Session::ModelPanel()
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	return pcImpl->ModelPanel();
}

//== Mouse 관련 함수 =================================================================================

void KERNEL::Session::MouseSignal(Json::Object & cInObject)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
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
			//pcImpl->GetCanvas().MButtonDown(nFlag, x, y);
			break;

		case Signal::View::Action::OnMButtonUp:
			//pcImpl->GetCanvas().MButtonUp(nFlag, x, y);
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

void KERNEL::Session::MouseMove(int nFlag, int x, int y)
{
	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->MouseMove(nFlag, x, y);
}

void KERNEL::Session::LButtonDown(int nFlag, int x, int y)
{
	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->LButtonDown(nFlag, x, y);
}

void KERNEL::Session::LButtonUp(int nFlag, int x, int y)
{
	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->LButtonUp(nFlag, x, y);
}

void KERNEL::Session::RButtonDown(int nFlag, int x, int y)
{
	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	Command::Event cEvent(pcImpl->Window());
	cEvent.SetPoint(H3DF::Operator::Event::Type::RButtonDown, x, y, pcImpl->MouseMapFlags(nFlag));

	pcImpl->Camera().RButtonDown(cEvent);
}

void KERNEL::Session::RButtonUp(int nFlag, int x, int y)
{
	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	Command::Event cEvent(pcImpl->Window());
	cEvent.SetPoint(H3DF::Operator::Event::Type::RButtonUp, x, y, pcImpl->MouseMapFlags(nFlag));

	pcImpl->Camera().RButtonUp(cEvent);
}

void KERNEL::Session::MouseWheel(int nFlag, int x, int y, Json::Object & cInObject)
{
	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->m_nMouseWhellStartTick = GetTickCount();

	int zDelta = cInObject.GetInteger(SKW_DELTA, -120);
	Json::Array & cArray = cInObject.GetArray(SKW_RECT);
	int nLeft = cArray[0]->ToInteger();
	int nTop = cArray[1]->ToInteger();

	pcImpl->GetCanvas().GetFrontView().SuppressUpdate(true);

	// Control Flag을 추가해서 ComputeReasonableTarget이란 함수를 사용해서 Whell Zomm할때 Entity를 선택하는 과정을 생략함.
	// nFlag |= MK_CONTROL;

	Command::Event cEvent(pcImpl->Window());
	cEvent.SetPoint(H3DF::Operator::Event::Type::MouseWheel, x - nLeft, y - nTop, pcImpl->MouseMapFlags(nFlag));
	cEvent.SetMouseWheelDelta(zDelta);

	pcImpl->Camera().MouseWheel(cEvent);

	pcImpl->Select().DrawSnapItems();

	pcImpl->GetCanvas().GetFrontView().SuppressUpdate(false);

	pcImpl->GetCanvas().GetFrontView().Update();
}

//== Keyboard 관련 함수 ==============================================================================

void KERNEL::Session::KeyboardSignal(Json::Object & cInObject)
{
	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

#ifdef _DEBUG
	Json::Object cTestObject(cInObject);
	CString strText;
	cTestObject.Stringify(strText);
#endif

	//pcImpl->Select()

	pcImpl->GetCanvas().KeyboardInput(cInObject);
}

void KERNEL::Session::CancelSignal()
{
	SessionImpl * pcImpl = dynamic_cast<SessionImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->Select().UnhighlightEverything();
}

//== View 관련 함수 ==================================================================================
void KERNEL::Session::SetViewControl(int nId)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
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

//== Object Snap 관련 함수 ===========================================================================
void KERNEL::Session::SetObjectSnap(int nId)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
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

//== Selection Fiter 관련 함수 =======================================================================
void KERNEL::Session::SetSelectionFilter(int nId)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
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

//== Command 관련 함수 ===============================================================================

// 1. 처리할 Command를 설정한다. 여기서 Command를 설정하고, 기존 Command가 있으면 마무리 하도록 한다.
void KERNEL::Session::SetCommand(int nId)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->SetCommand(nId);
}

//== View Style 관련 함수 ============================================================================

void KERNEL::Session::SetViewStyle(int nStyleId)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nStyleId)
	{
		case HOME_3D_CMD_ViewStyle_Shade:
			pcImpl->GetCanvas().GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::Gouraud);
			break;

		case HOME_3D_CMD_ViewStyle_ShadeWithEdges:
			pcImpl->GetCanvas().GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::GouraudWithLines);
			break;

		case HOME_3D_CMD_ViewStyle_Wireframe:
			pcImpl->GetCanvas().GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::Wireframe);
			break;

		case HOME_3D_CMD_ViewStyle_HiddenLineRemove:
			pcImpl->GetCanvas().GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::HiddenLine);
			break;

		case HOME_3D_CMD_ViewStyle_Tessellated:
			pcImpl->GetCanvas().GetFrontView().SetRenderingMode(H3DF::Rendering::Mode::Tessellated);
			break;

		default:
			assert(false);
			break;
	}
}

//== Visibility 관련 함수 ============================================================================
void KERNEL::Session::SetVisibility(int nId)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->SetVisibility(nId);
}

//== View Direction 관련 함수 =======================================================================

void KERNEL::Session::SetViewDirection(int nDirectionId)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	switch (nDirectionId)
	{
		case HOME_3D_CMD_ViewDirection_Top:
			pcImpl->GetCanvas().GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::top);
			break;

		case HOME_3D_CMD_ViewDirection_Front:
			pcImpl->GetCanvas().GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::front);
			break;

		case HOME_3D_CMD_ViewDirection_Left:
			pcImpl->GetCanvas().GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::left);
			break;

		case HOME_3D_CMD_ViewDirection_Bottom:
			pcImpl->GetCanvas().GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::bottom);
			break;

		case HOME_3D_CMD_ViewDirection_Back:
			pcImpl->GetCanvas().GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::back);
			break;

		case HOME_3D_CMD_ViewDirection_Right:
			pcImpl->GetCanvas().GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::right);
			break;

		case HOME_3D_CMD_ViewDirection_Iso:
			pcImpl->GetCanvas().GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::px_py_pz);
			break;

		case HOME_3D_CMD_ViewDirection_SeIso:
			pcImpl->GetCanvas().GetFrontView().SetViewDirection(H3DF::ViewDirection::Mode::nx_py_pz);
			break;

		case HOME_3D_CMD_ViewDirection_Perspective:
			assert(false);
			break;

	}
}

//== Command 관련 함수 ===============================================================================

// 1. Request Value 처리
void KERNEL::Session::CommandRequest(Json::Object & cInObject)
{
	SessionImpl * pcImpl = (SessionImpl *) m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->CommandRequest(cInObject);
}

// 1. Change Value 처리
void KERNEL::Session::CommandChange(Json::Object & cInObject)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->CommandChange(cInObject);
}

//== Panel 관련 함수 =================================================================================
void KERNEL::Session::ModelPanelSignal(Json::Object & cInObject)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	// Delivery는 ModelPanel 선언할 때 이미 할당함.
	pcImpl->ModelPanel().Signal(cInObject);
}

//== 임시 Test용 함수 ================================================================================
void KERNEL::Session::TestCommand(int nId)
{
	SessionImpl * pcImpl = (SessionImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	switch (nId)
	{
		case CUSTOM_3D_CMD_SYSONG_Test1:
			pcImpl->SetVisibility(nId);
			break;

		case CUSTOM_3D_CMD_SYSONG_Test2: {
			CString strFilePathName = H3DF::Utility::GetExecuteDirectory() + L"Image\\Texture\\zebra.jpeg";
			H3DF::Image::ImportOptionsKit cOptions;
			cOptions.SetFormat(H3DF::Image::Format::Jpeg);
			H3DF::ImageKit cImage = H3DF::Image::File::Import(strFilePathName, cOptions);

			H3DF::PortfolioKeyArray arPortfolios;
			H3DF::PortfolioKey cPortfolio;
 			H3DF::ImageDefinition cImageDefinition;
 			H3DF::TextureOptionsKit cTextureOptionsKit;

			H3DF::SegmentKey cModel = pcImpl->GetCanvas().GetFrontView().GetAttachedModel().GetSegmentKey();
			cModel.GetPortfolioControl().Show(arPortfolios);
			cPortfolio = arPortfolios[0];
			
			cImageDefinition = cPortfolio.DefineImage("zebra_texture", cImage);

 			cTextureOptionsKit.SetParameterizationSource(H3DF::Material::Texture::Parameterization::ReflectionVector);
			cTextureOptionsKit.SetDownSampling(false);

 			cPortfolio.DefineTexture("zebra", cImageDefinition, cTextureOptionsKit);

			cModel.GetMaterialMappingControl().SetFaceTexture("zebra", H3DF::Material::Texture::Channel::EnvironmentTexture);

		} break;

		default:
			break;
	}
}