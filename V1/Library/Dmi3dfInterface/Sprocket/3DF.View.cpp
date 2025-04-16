#include "stdafx.h"

#include "3DF.View.h"
#include "Impl/3DF.View.Impl.h"

#include "../Signal.Connector.h"

#include <Common_Define.h>
#include <Path.h>

#include "3DF.Canvas.h"
#include "Impl/CanvasImpl.h"


#include "3DF.Model.h"
#include "Impl/ModelImpl.h"

#include "../3DF/Segment.h"
#include "../3DF/Impl/SegmentImpl.h"
#include "../3DF/Camera.h"
#include "../3DF/Visibility.h"
#include "../3DF/VisualEffects.h"

#include "../3DF/Window.h"
#include "../3DF/Impl/WindowImpl.h"

#include "../3DF/Highlight.h"
#include "../3DF/Impl/HighlightImpl.h"

#include "../3DF/Facility.AppOptions.h"

#include "../3DF/3DF.Utility.h"

#include "../Import/DLL.Interface.h"

#include <HIOUtilityHsf.h>
#include <HConstantFrameRate.h>
#include <HEventManager.h>
//#include <HIOUtilityPointCloud.h>

#include <chrono>

#include "LogManager.h"

#include "../3DF/PointCloud.h"

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

Model & H3DF::View::GetAttachedModel() const
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetAttachedModel();
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

	return pcImpl->GetAttachedModel().GetPortfolioKey();
}

PortfolioKey H3DF::View::GetPortfolioKey()
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->GetAttachedModel().GetPortfolioKey();
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

	if (H3DF::Type::None == pcViewImpl->GetWindowKey().Type()) {
		DEBUG_RETURN;
	}

	pcViewImpl->m_eRenderingMode = eInMode;

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) pcViewImpl->GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);
	
	SegmentKey cViewKey = pcViewImpl->GetSegmentKey();
	SegmentKey cSceneKey(pcBaseView->GetSceneKey());

	if (H3DF::Rendering::Mode::HiddenLine == eInMode) {
		pcWindowImpl->SetWindowBackGroundColor(RGB(255, 255, 255), RGB(255, 255, 255));
	}
	else {
		pcWindowImpl->SetWindowBackGroundColor(TheKenel.Appearance.BackgroundColor.Top, TheKenel.Appearance.BackgroundColor.Bottom);
	}

	Model & cModel = GetAttachedModel();
	ModelImpl * pcModelImpl = static_cast<ModelImpl *>(cModel.GetImpl());
	DEBUG_VALID(pcModelImpl);

	// VisibilityControl SetFaces을 설정
	if (H3DF::Rendering::Mode::Wireframe == eInMode) {
		pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetFaces(false);
	}
	else {
		pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetFaces(true);
	}
	
	// VisibilityControl SetLines을 설정
	switch (eInMode)
	{
		case H3DF::Rendering::Mode::Gouraud:
		case H3DF::Rendering::Mode::Phong:
			pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetLines(false);
			break;

		case H3DF::Rendering::Mode::GouraudWithLines:
		case H3DF::Rendering::Mode::PhongWithLines:
		case H3DF::Rendering::Mode::HiddenLine:
		case H3DF::Rendering::Mode::FastHiddenLine:
		case H3DF::Rendering::Mode::Wireframe:
		case H3DF::Rendering::Mode::Tessellated:
			pcModelImpl->ShowStyleSegment().GetVisibilityControl().SetLines(true);
			break;
	}

	// Rendering Mode 설정.
	switch (eInMode)
	{
		case H3DF::Rendering::Mode::Gouraud:
		case H3DF::Rendering::Mode::GouraudWithLines:
			pcBaseView->RenderGouraud();
			break;

		case H3DF::Rendering::Mode::Phong:
		case H3DF::Rendering::Mode::PhongWithLines:
			pcBaseView->RenderPhong();
			break;

		case H3DF::Rendering::Mode::HiddenLine: {
			HConstantFrameRate * pcFramerate = pcBaseView->GetConstantFrameRateObject();
			pcFramerate->Stop();
			pcFramerate->Shutdown();

			pcBaseView->SetRenderMode(HRenderBRepHiddenLine, true);
			cSceneKey.GetMaterialMappingControl().SetEdgeColor(RGBAColor(0, 0, 0));
		} break;

		case H3DF::Rendering::Mode::FastHiddenLine: {
			HConstantFrameRate * pcFramerate = pcBaseView->GetConstantFrameRateObject();
			pcFramerate->Stop();
			pcFramerate->Shutdown();

			pcBaseView->SetRenderMode(HRenderHiddenLineFast, true);
		} break;

		case H3DF::Rendering::Mode::Wireframe:
			pcBaseView->RenderBRepWireframe();
			break;

		case H3DF::Rendering::Mode::Tessellated:
			pcBaseView->RenderGouraud();
			break;
	}

	// 이 위치에서 실행되어야 정상적으로 표현됨.
	if (H3DF::Rendering::Mode::Tessellated == eInMode) {
		cSceneKey.GetVisibilityControl().SetEdges(true);
	}
	else {
		cSceneKey.GetVisibilityControl().SetEdges(false);
	}

	pcBaseView->Update();
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

	if (H3DF::Type::None == pcImpl->GetWindowKey().Type()) {
		DEBUG_RETURN;
	}

	WindowKeyImpl * pcWindowImpl = (WindowKeyImpl *) pcImpl->GetWindowKey().GetImpl();
	DEBUG_VALID(pcWindowImpl);

	BaseView * pcBaseView = pcWindowImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	pcBaseView->SetViewDirection(eInMode);
}

void H3DF::View::SaveHsfFile(CString strFilePathName, Canvas * pcHoopsView)
{
	ViewImpl * pcImpl = static_cast<ViewImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	HIOUtilityHsf cUtilityHsf;

	BaseView * pcBaseView = pcImpl->GetBaseView();
	DEBUG_VALID(pcBaseView);

	HC_KEY nModelKey = pcBaseView->GetModelKey();

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

// Returns the status of the simple reflection
bool H3DF::View::GetSimpleReflection()
{
	ViewImpl * pcViewImpl = (ViewImpl *)m_pcImpl;
	DEBUG_VALID(pcViewImpl);

	return pcViewImpl->GetSimpleReflection();
}

// Smoothly moves the camera from the current position to the one specified by the user.
void H3DF::View::SmoothTransition(H3DF::CameraKit const & cInCamera)
{
	ViewImpl * pcImpl = (ViewImpl *)m_pcImpl;
	DEBUG_VALID(pcImpl);

	pcImpl->GetBaseView()->InvalidateSceneBounding();

	H3DF::SegmentKey cScene(pcImpl->GetBaseView()->GetSceneKey());

	H3DF::CameraKit cOldCamera;
	if (false == cScene.ShowCamera(cOldCamera)) {
		DEBUG_STOP;
		return;
	}

	Point cOldPosition;
	cOldCamera.ShowPosition(cOldPosition);

	Point cOldTarget;
	cOldCamera.ShowTarget(cOldTarget);

	Vector cOldUpVector;
	cOldCamera.ShowUpVector(cOldUpVector);

	float fOldWidth, fOldHeight;
	cOldCamera.ShowField(fOldWidth, fOldHeight);

	Point cNewPosition;
	cInCamera.ShowPosition(cNewPosition);

	Point cNewTarget;
	cInCamera.ShowTarget(cNewTarget);

	Vector cNewUpVector;
	cInCamera.ShowUpVector(cNewUpVector);

	float fNewWidth, fNewHeight;
	cInCamera.ShowField(fNewWidth, fNewHeight);

	HUtility::SmoothTransition(
		(HPoint *)&cOldPosition,
		(HPoint *)&cOldTarget,
		(HPoint *)&cOldUpVector,
		fOldWidth, fOldHeight,
		(HPoint *)&cNewPosition,
		(HPoint *)&cNewTarget,
		(HPoint *)&cNewUpVector,
		fNewWidth, fNewHeight,
		pcImpl->GetBaseView());

	//pcImpl->GetBaseView()->ZoomToExtents();

	pcImpl->GetBaseView()->SetZoomLimit();
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

bool H3DF::View::DoDynamicHighlighting(H3DF::HighlightControl & cHighlightControl, WindowPoint cInWindowPoint)
{
	int res, offset1, offset2, offset3;
	char pathname[MVO_SEGMENT_PATHNAME_BUFFER], type[MVO_BUFFER_SIZE];
	HC_KEY primitive;

	HPoint cMousePos;
	cMousePos.x = cInWindowPoint.x;
	cMousePos.y = cInWindowPoint.y;
	cMousePos.z = 0;

// 	if (!m_bDynamicHighlighting || GetSuppressUpdateTick() || GetSuppressUpdate() || !GetModel()->GetFileLoadComplete())
// 		return;

	H3DF::HighlightControlImpl * pcHighlightControlImpl = (H3DF::HighlightControlImpl *) cHighlightControl.GetImpl();
	
	ViewImpl * pcImpl = dynamic_cast<ViewImpl *> (m_pcImpl);
	DEBUG_VALID(pcImpl);

	HC_KEY nViewKey = pcHighlightControlImpl->GetBaseView()->GetViewKey();
	HSelectionSet * pcHighlight = pcHighlightControlImpl->SelectionSet();
	//HSelectionSet * pcHighlight = pcImpl->GetBaseView()->GetHighlightSelection();// pcHighlightControlImpl->SelectionSet();

	HC_Open_Segment_By_Key(nViewKey);
	res = HC_Compute_Selection(".", "./scene/overwrite", "v, selection level = entity", cMousePos.x, cMousePos.y);
	HC_Close_Segment();

	// compute the selection using the HOOPS window coordinate of the pick location
	bool need_deselect = true;
	bool need_update = true;

	if (res) {
		HC_Show_Selection_Element(&primitive, &offset1, &offset2, &offset3);
		HC_Show_Selection_Pathname(pathname);

		int incl_count;
		int skey_count;
		char skey_type[MVO_BUFFER_SIZE];

		HC_Show_Selection_Keys_Count(&skey_count);

		HC_KEY * keys = new HC_KEY[skey_count];
		HC_KEY * incl_keys = new HC_KEY[skey_count];
		HC_Show_Selection_Keys(&skey_count, keys);

		incl_count = 0;
		for (int i = skey_count - 1; i >= 0; i--) {
			HC_Show_Key_Type(keys[i], skey_type);
			if (strstr(skey_type, "include")) {
				incl_keys[incl_count] = keys[i];
				incl_count++;
			}
			else if (streq(skey_type, "reference")) {
				primitive = keys[i];
				break;
			}
		}

		// Get the type of the selected
		HC_Show_Key_Type(primitive, type);

		// if we have a shell with visible faces, we may need to select regions
		if (streq(type, "shell") && offset3 != -1) {
			int region;
			int lowest = 0;
			int highest = 0;

			if (pcHighlight->GetAllowRegionSelection())
				HC_Show_Region_Range(primitive, &lowest, &highest);

			if (lowest != highest || lowest > 0) {
				HC_Open_Geometry(primitive);
				{
					HC_Open_Face(offset3);
					{
						HC_Show_Region(&region);
					}
					HC_Close_Face();
				}
				HC_Close_Geometry();

				need_deselect = false;

				if (!pcHighlight->IsRegionSelected(primitive, incl_count, incl_keys, region)) {
					pcHighlight->DeSelectAll();
					pcHighlight->SelectRegion(primitive, incl_count, incl_keys, region, false);
				}
				else
					need_update = false;

				goto DONE;
			}

			// NON-REGION SELECT FALLS THROUGH
		}

		need_deselect = false;

		if (!pcHighlight->IsSelected(primitive, incl_count, incl_keys)) {
			if (pcHighlight->GetSelectionLevel() !=
				HSelectSegment) // never should fail for dynamic highlighting, but let's be nice and check
			{
				// the key is to a geometric entity.  If we are in segment selection mode,
				// then we need to get the key to its parent segment.

				HC_Show_Key_Type(primitive, type);

				if (!streq("segment", type)) {
					char segname[MVO_BUFFER_SIZE];
					HC_KEY segkey;

					segkey = HC_KShow_Owner_Original_Key(primitive);
					HC_Show_Owner_By_Key(primitive, segname);

					// climb up one more level if this is the temporary highlight key
					if (pcHighlight->IsHighlightSegment(segkey)) {
						segkey = HC_KShow_Owner_Original_Key(segkey);
						HC_Show_Owner_By_Key(segkey, segname);
					}
				}
			}
			pcHighlight->DeSelectAll();
			pcHighlight->Select(primitive, incl_count, incl_keys, false);
		}
		else
			need_update = false;

	DONE:
		delete[] keys;
		delete[] incl_keys;
	}

	if (need_deselect) {
		pcHighlight->DeSelectAll();
	}

	if (need_update) {
		pcHighlightControlImpl->GetBaseView()->ForceUpdate();
	}

	return true;
}
