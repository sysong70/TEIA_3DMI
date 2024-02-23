#include "stdafx.h"

#include "3DF.View.h"
#include "Impl/ViewImpl.h"

#include "Signal.Connector.h"

#include <Common_Define.h>
#include <Path.h>

#include "3DF.Canvas.h"
#include "Impl/CanvasImpl.h"

#include "3DF/Segment.h"
#include "3DF/Impl/SegmentImpl.h"
#include "3DF/Visibility.h"
#include "3DF/VisualEffects.h"

#include "3DF/Facility.AppOptions.h"

#include "3DF/3DF.Utility.h"

#include "Import/DLL.Interface.h"

#include <HIOUtilityHsf.h>
#include <HConstantFrameRate.h>
#include <HEventManager.h>
//#include <HIOUtilityPointCloud.h>

#include <chrono>

#include "LogManager.h"

#include "3DF/PointCloud.h"

using namespace H3DF;
using namespace std::chrono;

#define TheKenel TheAppOptions.Kernel
#define ThePreset TheAppOptions.Preset

H3DF::View::View()
{
	m_pcImpl = new ViewImpl();
	if (nullptr == m_pcImpl) {
		assert(false);
	}
}

H3DF::View::View(View const & cInThat)
{
	m_pcImpl = new ViewImpl();
	if (nullptr == m_pcImpl) {
		assert(false);
	}

	Set(cInThat);
}

H3DF::View::~View()
{

}

void H3DF::View::Set(View const & cInThat)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	ViewImpl * pcInThatImpl = (ViewImpl *)cInThat.m_pcImpl;
	pcImpl->Copy(pcInThatImpl);
}

View const & H3DF::View::operator = (View const & cInThat)
{
	Set(cInThat);
	return *this;
}

void H3DF::View::Update() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (pcImpl->GetBaseView()->GetViewActive() && !pcImpl->GetBaseView()->GetSuppressUpdate())
	{
		HC_Control_Update_By_Key(pcImpl->GetBaseView()->GetViewKey(), "redraw everything");
		pcImpl->GetBaseView()->GetConstantFrameRateObject()->SetActivityType(GeneralActivity);

		if (false == pcImpl->GetBaseView()->GetFirstUpdate()) {
			pcImpl->GetBaseView()->ForceUpdate();
		}
		else {
			pcImpl->GetBaseView()->Update();
		}
	}
}

void H3DF::View::Update(Json::Object & cInObject) const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	if (false == pcImpl->IsInitNavigationCube()) {
		Json::Array & cArray = cInObject.GetArray(SKW_RECT);
		int nLeft = cArray[0]->ToInteger();
		int nTop = cArray[1]->ToInteger();
		int nRight = cArray[2]->ToInteger();
		int nBottom = cArray[3]->ToInteger();

		pcImpl->InitNavigationCube(nRight, nBottom);
	}

	if (pcImpl->GetBaseView()->GetViewActive() && !pcImpl->GetBaseView()->GetSuppressUpdate())
	{
		HC_Control_Update_By_Key(pcImpl->GetBaseView()->GetViewKey(), "redraw everything");
		pcImpl->GetBaseView()->GetConstantFrameRateObject()->SetActivityType(GeneralActivity);

//		pcCanvas->GetIntRectangle(&rectangle);
// 		m_pHView->Notify(HSignalPaint, &rectangle);
// 		m_pHView->ResetIdleTime();

		if (false == pcImpl->GetBaseView()->GetFirstUpdate()) {
			pcImpl->GetBaseView()->ForceUpdate();
		}
		else {
			pcImpl->GetBaseView()->Update();
		}
	}
}

void H3DF::View::Update(Json::Object & cInObject, Window::UpdateType eInType, H3DF::Time dInTimeLimit) const
{
	Update(cInObject);
}

void H3DF::View::SuppressUpdate(bool bSuppress)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->GetBaseView()->SetSuppressUpdate(bSuppress);
}

void H3DF::View::Destruct() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	if (nullptr != pcImpl->m_pcBaseView) {
		pcImpl->m_pcBaseView->SetSuppressUpdate(true);
		pcImpl->m_pcBaseView->SetModel(nullptr);
		delete pcImpl->m_pcBaseView;
	}

	if (nullptr != pcImpl->m_pcWindow) {
		delete pcImpl->m_pcWindow;
	}
}

void H3DF::View::Resize(int x, int y)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) { DEBUG_RETURN; }

	pcImpl->Resize(x, y);
}

Model & H3DF::View::GetAttachedModel() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetAttachedModel();
}

WindowKey & H3DF::View::GetWindowKey() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	DEBUG_VALID(pcImpl->m_pcWindow);

	return *pcImpl->m_pcWindow;
}

SegmentKey H3DF::View::GetSegmentKey()
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

SegmentKey const H3DF::View::GetSegmentKey() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetSegmentKey();
}

SegmentKey H3DF::View::GetModelOverrideSegmentKey()
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetModelKey();
}

SegmentKey const H3DF::View::GetModelOverrideSegmentKey() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetModelKey();
}

PortfolioKey const H3DF::View::GetPortfolioKey() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cPortfolioKey;
}

PortfolioKey H3DF::View::GetPortfolioKey()
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cPortfolioKey;
}

NavigationCube & H3DF::View::GetNavigationCube() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetNavigationCube();
}

void H3DF::View::SetSuppressUpdate(bool bInState)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->GetBaseView()->SetSuppressUpdate(bInState);
}

//== Command 관련 함수 ===========================================================================

// 명령어 취소 함수, Select된 Object도 취소됨.
void H3DF::View::CancelCommands()
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	//pcImpl->DeSelectAll();
}

void H3DF::View::CancelCommands() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	//pcImpl->DeSelectAll();
}

// == Action Function ==============================================================================

bool H3DF::View::Char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->Char(nChar, nRepCnt, nFlags);
}

bool H3DF::View::KeyboardInput(Json::Object & input)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->KeyboardInput(input);
}

bool H3DF::View::ExecuteKeyboardSignal(int nAction, Json::Object& cInObject)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) {
		RETURN_FALSE;
	}

	return pcImpl->KeyboardInput(cInObject);
}

//== Select 관련 함수 ========================================================================
void H3DF::View::SetSubentitySelectLevel()
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	//pcImpl->SetSubentitySelectLevel();
}


//== View Style 관련 함수 ====================================================================
void H3DF::View::SetRenderingMode(Rendering::Mode eInMode)
{
	ViewImpl * pcViewImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcViewImpl);

	pcViewImpl->m_eRenderingMode = eInMode;

	BaseView * pcView = pcViewImpl->GetBaseView();
	
	SegmentKey cViewKey = pcViewImpl->GetSegmentKey();
	SegmentKey cSceneKey(pcView->GetSceneKey());

	if (H3DF::Rendering::Mode::HiddenLine == eInMode) {
		pcViewImpl->SetWindowBackGroundColor(RGB(255, 255, 255), RGB(255, 255, 255));
	}
	else {
		pcViewImpl->SetWindowBackGroundColor(TheKenel.Appearance.BackgroundColor.Top, TheKenel.Appearance.BackgroundColor.Bottom);
	}
	
	switch (eInMode)
	{
		case H3DF::Rendering::Mode::Gouraud:
			pcView->RenderGouraud();
			GetModelOverrideSegmentKey().GetVisibilityControl().SetLines(false);
			cSceneKey.GetVisibilityControl().SetEdges(false);
			break;

		case H3DF::Rendering::Mode::GouraudWithLines:
			pcView->RenderGouraud();
			GetModelOverrideSegmentKey().GetVisibilityControl().SetLines(true);
			cSceneKey.GetVisibilityControl().SetEdges(false);
			break;

		case H3DF::Rendering::Mode::Flat:
			break;

		case H3DF::Rendering::Mode::FlatWithLines:
			break;

		case H3DF::Rendering::Mode::Phong: {
			pcView->RenderPhong();
			GetModelOverrideSegmentKey().GetVisibilityControl().SetLines(false);
			cSceneKey.GetVisibilityControl().SetEdges(false);
		} break;

		case H3DF::Rendering::Mode::PhongWithLines:
			pcView->RenderPhong();
			GetModelOverrideSegmentKey().GetVisibilityControl().SetLines(true);
			cSceneKey.GetVisibilityControl().SetEdges(false);
			break;

		case H3DF::Rendering::Mode::HiddenLine: {
			HConstantFrameRate * pcFramerate = pcView->GetConstantFrameRateObject();
			pcFramerate->Stop();
			pcFramerate->Shutdown();

			GetModelOverrideSegmentKey().GetVisibilityControl().SetLines(true);

			pcView->SetRenderMode(HRenderBRepHiddenLine, true);
			cSceneKey.GetMaterialMappingControl().SetEdgeColor(RGBAColor(0, 0, 0));
		} break;

		case H3DF::Rendering::Mode::FastHiddenLine: {
			HConstantFrameRate * pcFramerate = pcView->GetConstantFrameRateObject();
			pcFramerate->Stop();
			pcFramerate->Shutdown();

			pcView->SetRenderMode(HRenderHiddenLineFast, true);
			cSceneKey.GetVisibilityControl().SetEdges(false);
		} break;

		case H3DF::Rendering::Mode::Wireframe: {
			pcView->RenderBRepWireframe();
			GetModelOverrideSegmentKey().GetVisibilityControl().SetLines(true);
			cSceneKey.GetVisibilityControl().SetEdges(false);
		} break;

		case H3DF::Rendering::Mode::Tessellated:
			pcView->RenderPhong();
			cSceneKey.GetVisibilityControl().SetEdges(true);
			break;

		default:
			assert(false);
			break;
	}

	pcView->Update();
}

Rendering::Mode H3DF::View::GetRenderingMode() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_eRenderingMode;
}

void H3DF::View::SetViewDirection(ViewDirection::Mode eInMode)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->GetBaseView()->SetViewDirection(eInMode);
}

void H3DF::View::SaveHsfFile(CString strFilePathName, Canvas * pcHoopsView)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	HIOUtilityHsf cUtilityHsf;

	HC_KEY nModelKey = pcImpl->m_pcBaseView->GetModelKey();

	HC_Open_Segment_By_Key(nModelKey);

	HOutputHandlerOptions cOptions;
	HStreamFileToolkit * mytool = new HStreamFileToolkit;
	cOptions.ExtendedData(mytool);

	int sflags = 0;
	sflags |= TK_Full_Resolution_Vertices;
	sflags |= TK_Full_Resolution_Normals;
	sflags |= TK_Full_Resolution_Parameters;

	mytool->SetWriteFlags(sflags);

	HFileOutputResult eResult = cUtilityHsf.FileOutputByKey(strFilePathName, nModelKey, &cOptions);

	HC_Close_Segment();

	delete mytool;
}

// Turns simple shadows on or off, automatically calculating shadow plane and light direction
// param: in_state controls whether simple shadows are on or off
// param: fInPercentOffset distance to add between the model's bounding box and the position of the shadow plane If not specified, it is set to 5%.
void H3DF::View::SetSimpleShadow(bool bInState, float fInPercentOffset)
{
	ViewImpl * pcViewImpl = (ViewImpl *)m_pcImpl;
	DEBUG_VALID(pcViewImpl);

	SegmentKey cViewSegment = pcViewImpl->GetSegmentKey();

	const float fOpacity = 1.0f;
	const unsigned int nResolution = 512;
	const unsigned int nBlurring = 20;

	// Set opacity in simple shadow color
	float fGray = 0.25f;
	H3DF::RGBAColor cColor(fGray, fGray, fGray, fOpacity);
// 	if (true == cViewSegment.GetVisualEffectsControl().ShowSimpleShadowColor(cColor)) {
// 		cColor.alpha = fOpacity;
// 	}

	cViewSegment.GetVisualEffectsControl()
		.SetSimpleShadow(bInState, VisualEffects::ShadowMode::Soft, nResolution, nBlurring)
		.SetSimpleShadowColor(cColor);
}

// Returns the status of the simple shadows
bool H3DF::View::GetSimpleShadow()
{
	ViewImpl * pcViewImpl = (ViewImpl *)m_pcImpl;
	DEBUG_VALID(pcViewImpl);

	if (H3DF::VisualEffects::ShadowMode::None == (H3DF::VisualEffects::ShadowMode)pcViewImpl->GetBaseView()->GetShadowMode()) {
		return false;
	}

	return true;
}

// Turns simple reflection on or off, automatically calculating reflection plane and light direction
// param: in_state controls whether simple reflection is on or off
// param: in_percent_offset distance to add between the model's bounding box and the position of the reflection plane If not specified, it is set to 5%.
void H3DF::View::SetSimpleReflection(bool bInState, float fInPercentOffset)
{
	ViewImpl * pcViewImpl = (ViewImpl *)m_pcImpl;
	DEBUG_VALID(pcViewImpl);

	pcViewImpl->SetSimpleReflection(bInState);

	SegmentKey cViewSegment = pcViewImpl->GetSegmentKey();

	float fOpacity = TheKenel.VisualEffects.PlaneReflection.GetOpacity();
	int nBlurring = TheKenel.VisualEffects.PlaneReflection.GetBlurring();
	bool bFading = TheKenel.VisualEffects.PlaneReflection.Fading;

	cViewSegment.GetVisualEffectsControl().SetSimpleReflection(bInState, fOpacity, nBlurring, bFading);
}

/*! Returns the status of the simple reflection */
bool H3DF::View::GetSimpleReflection()
{
	ViewImpl * pcViewImpl = (ViewImpl *)m_pcImpl;
	DEBUG_VALID(pcViewImpl);

	return pcViewImpl->GetSimpleReflection();
}


void H3DF::View::LoadPointCloudFile(CString strFilePathName)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	if (nullptr == pcImpl) {
		DEBUG_RETURN;
	}

	SegmentKey cViewKey(pcImpl->GetBaseView()->GetViewKey());
	SegmentKeyImpl::LocalOpen(cViewKey); {
		HC_Set_Driver_Options("eye dome lighting = (on, strength=1.0)");
	} SegmentKeyImpl::LocalClose(cViewKey);

	HPointCloudOptions cPointCloudOptions;
	cPointCloudOptions.m_highIntensityValue = 0;
	cPointCloudOptions.m_lowIntensityValue = -2000;
	cPointCloudOptions.m_maxShellSize = 10000;

	HInputHandlerOptions cOptions;
	cOptions.m_pExtendedData = &cPointCloudOptions;

	SegmentKey cModelKey(pcImpl->GetBaseView()->GetModelKey());
	SegmentKey cPointCloudSegment = cModelKey.Subsegment("3dmi_point_cloud");

	PointCloud cPointCloud;
	
	LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"PointCloud Open");

	// 라이브러리를 사용해야 하므로 미리 cPointCloudSegment를 Open하도록 한다.
	SegmentKeyImpl::LocalOpen(cPointCloudSegment); {
		cPointCloud.FileInputByKey(Utility::ToChar(strFilePathName), cPointCloudSegment.KeyValue(), &cOptions);

 		HC_UnSet_Marker_Symbol();
 		HC_Set_Marker_Size(0.2);

	} SegmentKeyImpl::LocalClose(cPointCloudSegment);

	LogManager::Log(LOGMANAGER_3DF_LOG_ID, L"PointCloud Close");

	// Point Clouse Segment의 하부를 검색해서 색상을 변경함.
	// Library에서 나오는 색상은 기본적으로 Black으로 나옴.
	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetVertexColor(RGBAColor(0.75, 1.0, 0.75)); // Gray Color 설정
	Utility::ChangeSubSegmentColor(cPointCloudSegment, cMaterialMapping, true);
}

