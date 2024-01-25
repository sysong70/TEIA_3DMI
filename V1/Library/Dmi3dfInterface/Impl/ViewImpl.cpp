#include "StdAfx.h"

#include "ViewImpl.h"

#include "CanvasImpl.h"

#include "../3DF.Model.h"
#include "ModelImpl.h"

#include <hc.h>
#include <HTools.h>
#include <HBaseModel.h>
#include <HEventManager.h>
#include <HBhvBehaviorManager.h>
#include <HEventManager.h>
#include <HMarkupManager.h>
#include <HSharedKey.h>
#include <HUtilityGeomHandle.h>
#include <HEventListener.h>
#include <HOpCameraManipulate.h>
#include <HOpCameraOrbit.h>
#include <HOpCameraPan.h>
#include <HOpCameraZoom.h>
#include <HOpMoveHandle.h>
#include <HUndoManager.h>
#include <hic.h>
#include <HConstantFrameRate.h>

#include "Common_Define.h"

#include "../3DF/Selection.h"
#include "../3DF/Highlight.h"
#include "../3DF/SelectionSet.h"

#include "../3DF/NavigationCube.h"
#include "../3DF/3DF.Utility.h"
#include "../3DF/Facility.AppOptions.h"

#include "../3DF/3DF.Operator.CameraControl.h"
#include "../3DF/Operator.SelectArea.h"

#include "../3DF/Window.h"
#include "../3DF/Visibility.h"
#include "../3DF/Material.h"
#include "../3DF/LineAttribute.h"
#include "../3DF/Impl/SegmentImpl.h"

#include "../3DF/Database.h"
#include "../3DF/Portfolio.h"

#include "../Signal/Signal.h"

#define SEGMENT_TYPE						1
#define ENTITY_TYPE							2
#define SUBENTITY_TYPE						3
#define REGION_TYPE							4

#define	DEBUG_NO_WINDOWS_HOOK				0x00000040
#define DEBUG_STARTUP_CLEAR_BLACK			0x00004000
#define DEBUG_FORCE_SOFTWARE				0x01000000

#define H_VIEW_POINTER_INDEX				4155

#define UINT2bool(__uint__val)  (__uint__val > 0) ?  true: false
#define BOOL2bool(TRUE_Or_FALSE) (( TRUE_Or_FALSE == TRUE ) ? true : false)

using namespace H3DF;

#define TheKenel TheAppOptions.Kernel
#define ThePreset TheAppOptions.Preset

#define ColorValue(x) GetRValue(x) / 255.0f, GetGValue(x) / 255.0f, GetBValue(x) / 255.0f
#define ColorRGBA(x, alpha) GetRValue(x), GetGValue(x), GetBValue(x), (unsigned char)alpha

//== BaseView Class ================================================================================

H3DF::BaseView::BaseView(HBaseModel * model, const char * alias, const char * driver_type, const char * instance_name,
	void * window_handle, void * colormap, void * clip_override, void * window_handle_2, const char * driver_path)
	: HBaseView(model, alias, driver_type, instance_name, window_handle, colormap, clip_override, window_handle_2, driver_path)
{
}

void H3DF::BaseView::UpdateInternal(bool antialias, bool force_update)
{
	if (nullptr != m_pcNaviCube) {
		if (true == m_pcNaviCube->IsInitialized()) {
			m_pcNaviCube->Transform();
		}
	}

	HBaseView::UpdateInternal(antialias, force_update);
}

void H3DF::BaseView::SetViewDirection(H3DF::ViewDirection::Mode eViewMode, bool bFitWorld)
{
	H3DF::ViewDirection::Mode eOldViewMode = m_eViewMode;
	m_eViewMode = eViewMode;

	if (H3DF::ViewDirection::Mode::Unknown == eViewMode) {
		return;
	}

	HPoint target, camera, view;
	float fLength;

	HPoint cPosition, cTarget, cUpVector;
	float widtho, heighto;
	char projection[MVO_BUFFER_SIZE];

	HPoint cn, tn, un;
	float widthn, heightn;
	char lprojection[MVO_BUFFER_SIZE];

	HC_Open_Segment_By_Key(GetSceneKey()); {
		HC_PShow_Net_Camera(0, 0, &cPosition, &cTarget, &cUpVector, &widtho, &heighto, projection);
	}HC_Close_Segment();

	PrepareForCameraChange();

	if (true == bFitWorld) {
		FitWorld();
	}

	HC_Open_Segment_By_Key(GetSceneKey()); {

		HC_PShow_Net_Camera_Target(0, 0, &target.x, &target.y, &target.z);
		HC_PShow_Net_Camera_Position(0, 0, &camera.x, &camera.y, &camera.z);

		view.Set(camera.x - target.x, camera.y - target.y, camera.z - target.z);

		fLength = (float)HC_Compute_Vector_Length(&view);

		float fNewLen = fLength * 0.5774f;

		HPoint rightaxis;
		HC_Compute_Cross_Product(&m_FrontAxis, &m_TopAxis, &rightaxis);

		if (GetHandedness() == HandednessRight) {
			rightaxis.Set(-rightaxis.x, -rightaxis.y, -rightaxis.z);
		}

		float px = target.x + fNewLen * m_FrontAxis.x - fNewLen * rightaxis.x + fNewLen * m_TopAxis.x;
		float py = target.y + fNewLen * m_FrontAxis.y - fNewLen * rightaxis.y + fNewLen * m_TopAxis.y;
		float pz = target.z + fNewLen * m_FrontAxis.z - fNewLen * rightaxis.z + fNewLen * m_TopAxis.z;

		float fCos45 = cos(M_PI / 4);
		float fLenCos = fLength * fCos45;

		HVector cLenFrontAxis = m_FrontAxis * fLength;
		HVector cLenTopAxis = m_TopAxis * fLength;
		HVector cLenRightAxis = rightaxis * fLength;

		HVector cVertexVector(fNewLen * m_FrontAxis.x, fNewLen * m_TopAxis.y, fNewLen * rightaxis.z);

		Point cSetPosition;
		Vector cSetUpVector;

		switch (m_eViewMode) {

			case H3DF::ViewDirection::Mode::right: {
				cSetPosition.Set(target.x + cLenFrontAxis.x, target.y + cLenFrontAxis.y, target.z + cLenFrontAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(0, -cUpVector.z, cUpVector.y);
				}
			} break;

			case H3DF::ViewDirection::Mode::left: {
				cSetPosition.Set(target.x - cLenFrontAxis.x, target.y + cLenFrontAxis.y, target.z + cLenFrontAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(0, cUpVector.z, -cUpVector.y);
				}
			} break;

			case H3DF::ViewDirection::Mode::front: {
				cSetPosition.Set(target.x + cLenTopAxis.x, target.y - cLenTopAxis.y, target.z + cLenTopAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(-cUpVector.z, 0, cUpVector.x);
				}
			} break;

			case H3DF::ViewDirection::Mode::back: {
				cSetPosition.Set(target.x + cLenTopAxis.x, target.y + cLenTopAxis.y, target.z + cLenTopAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(cUpVector.z, 0, -cUpVector.x);
				}
			} break;

			case H3DF::ViewDirection::Mode::top: {
				cSetPosition.Set(target.x + cLenRightAxis.x, target.y + cLenRightAxis.y, target.z - cLenRightAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 1, 0);
				}
				else {
					cSetUpVector.Set(-cUpVector.y, cUpVector.x, 0);
				}
			} break;

			case H3DF::ViewDirection::Mode::bottom: {
				cSetPosition.Set(target.x + cLenRightAxis.x, target.y + cLenRightAxis.y, target.z + cLenRightAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 1, 0);
				}
				else {
					cSetUpVector.Set(cUpVector.y, -cUpVector.x, 0);
				}
			} break;

			case H3DF::ViewDirection::Mode::py_nz: { // Back - Bottom
				cSetPosition.Set(target.x, target.y + fLenCos, target.z - fLenCos);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, fCos45, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.y - fCos45)) {
						cSetUpVector.Set(1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - 1)) {
						cSetUpVector.Set(0, -fCos45, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.y + fCos45)) {
						cSetUpVector.Set(-1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + 1)) {
						cSetUpVector.Set(0, fCos45, fCos45);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::py_pz: { // Top - Back
				cSetPosition.Set(target.x, target.y + fLenCos, target.z + fLenCos);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, -fCos45, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.y + fCos45)) {
						cSetUpVector.Set(1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - 1)) {
						cSetUpVector.Set(0, fCos45, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.y - fCos45)) {
						cSetUpVector.Set(-1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + 1)) {
						cSetUpVector.Set(0, -fCos45, fCos45);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::ny_pz: { // Top - Front
				cSetPosition.Set(target.x, target.y - fLenCos, target.z + fLenCos);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, fCos45, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.y - fCos45)) {
						cSetUpVector.Set(-1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + 1)) {
						cSetUpVector.Set(0, -fCos45, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.y + fCos45)) {
						cSetUpVector.Set(1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - 1)) {
						cSetUpVector.Set(0, fCos45, fCos45);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::ny_nz: { // Front - Bottom
				cSetPosition.Set(target.x, target.y - fLenCos, target.z - fLenCos);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, -fCos45, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.y + fCos45)) {
						cSetUpVector.Set(-1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + 1)) {
						cSetUpVector.Set(0, fCos45, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.y - fCos45)) {
						cSetUpVector.Set(1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - 1)) {
						cSetUpVector.Set(0, -fCos45, fCos45);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::nx_nz: { // Left - Bottom
				cSetPosition.Set(target.x - fLenCos, target.y, target.z - fLenCos);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-fCos45, 0, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.x + fCos45)) {
						cSetUpVector.Set(0, 1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						cSetUpVector.Set(fCos45, 0, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						cSetUpVector.Set(0, -1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y + 1)) {
						cSetUpVector.Set(-fCos45, 0, fCos45);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::nx_pz: { // top - Left
				cSetPosition.Set(target.x - fLenCos, target.y, target.z + fLenCos);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(fCos45, 0, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.x - fCos45)) {
						cSetUpVector.Set(0, 1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						cSetUpVector.Set(-fCos45, 0, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						cSetUpVector.Set(0, -1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y + 1)) {
						cSetUpVector.Set(fCos45, 0, fCos45);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::px_pz: { // Top - Right
				cSetPosition.Set(target.x + fLenCos, target.y, target.z + fLenCos);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-fCos45, 0, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.x + fCos45)) {
						cSetUpVector.Set(0, -1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y + 1)) {
						cSetUpVector.Set(fCos45, 0, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						cSetUpVector.Set(0, 1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						cSetUpVector.Set(-fCos45, 0, fCos45);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::px_nz: { // Right - Bottom
				cSetPosition.Set(target.x + fLenCos, target.y, target.z - fLenCos);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(fCos45, 0, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.x - fCos45)) {
						cSetUpVector.Set(0, -1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y + 1)) {
						cSetUpVector.Set(-fCos45, 0, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						cSetUpVector.Set(0, 1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						cSetUpVector.Set(fCos45, 0, fCos45);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::nx_py: { // Back - Left
				cSetPosition.Set(target.x - fLenCos, target.y + fLenCos, target.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					if (1e-6 > fabs(cUpVector.z - 1)) {
						cSetUpVector.Set(fCos45, fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						cSetUpVector.Set(0, 0, -1);
					}
					else if (1e-6 > fabs(cUpVector.z + 1)) {
						cSetUpVector.Set(-fCos45, -fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						cSetUpVector.Set(0, 0, 1);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::px_py: { // Right - Back
				cSetPosition.Set(target.x + fLenCos, target.y + fLenCos, target.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					if (1e-6 > fabs(cUpVector.z - 1)) {
						cSetUpVector.Set(fCos45, -fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						cSetUpVector.Set(0, 0, -1);
					}
					else if (1e-6 > fabs(cUpVector.z + 1)) {
						cSetUpVector.Set(-fCos45, fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						cSetUpVector.Set(0, 0, 1);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::px_ny: { // Front - Right
				cSetPosition.Set(target.x + fLenCos, target.y - fLenCos, target.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					if (1e-6 > fabs(cUpVector.z - 1)) {
						cSetUpVector.Set(-fCos45, -fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						cSetUpVector.Set(0, 0, -1);
					}
					else if (1e-6 > fabs(cUpVector.z + 1)) {
						cSetUpVector.Set(fCos45, fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						cSetUpVector.Set(0, 0, 1);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::nx_ny: { // Left - Front
				cSetPosition.Set(target.x - fLenCos, target.y - fLenCos, target.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					if (1e-6 > fabs(cUpVector.z - 1)) {
						cSetUpVector.Set(-fCos45, fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						cSetUpVector.Set(0, 0, -1);
					}
					else if (1e-6 > fabs(cUpVector.z + 1)) {
						cSetUpVector.Set(fCos45, -fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						cSetUpVector.Set(0, 0, 1);
					}
				}
			} break;

			case H3DF::ViewDirection::Mode::nx_py_nz: { // bottom - left - back
				cSetPosition.Set(target.x - cVertexVector.x, target.y + cVertexVector.y, -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, 0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.z, -cUpVector.x, -cUpVector.y);
				}
			} break;

			case H3DF::ViewDirection::Mode::nx_py_pz: { // top - back - left
				cSetPosition.Set(target.x - cVertexVector.x, target.y + cVertexVector.y, target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(-cUpVector.y, cUpVector.z, -cUpVector.x);
				}
			} break;

			case H3DF::ViewDirection::Mode::nx_ny_pz: { // top - left - front
				cSetPosition.Set(target.x - cVertexVector.x, -(target.y + cVertexVector.y), target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, 0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(-cUpVector.z, cUpVector.x, -cUpVector.y);
				}
			} break;

			case H3DF::ViewDirection::Mode::nx_ny_nz: { // bottom - front - left
				cSetPosition.Set(target.x - cVertexVector.x, -(target.y + cVertexVector.y), -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.y, cUpVector.z, cUpVector.x);
				}
			} break;

			case H3DF::ViewDirection::Mode::px_py_pz: { // ISO top - right - back
				cSetPosition.Set(target.x + cVertexVector.x, target.y + cVertexVector.y, target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.z, cUpVector.x, cUpVector.y);
				}
			} break;

			case H3DF::ViewDirection::Mode::px_py_nz: { // bottom - back - right
				cSetPosition.Set(target.x + cVertexVector.x, target.y + cVertexVector.y, -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, 0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.y, -cUpVector.z, -cUpVector.x);
				}

			} break;

			case H3DF::ViewDirection::Mode::px_ny_nz: { // bottom - right - front
				cSetPosition.Set(target.x + cVertexVector.x, -(target.y + cVertexVector.y), -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(-cUpVector.z, -cUpVector.x, cUpVector.y);
				}

			} break;

			case H3DF::ViewDirection::Mode::px_ny_pz: { // top - front - right
				cSetPosition.Set(target.x + cVertexVector.x, -(target.y + cVertexVector.y), target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, 0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(-cUpVector.y, -cUpVector.z, cUpVector.x);
				}

			} break;

			default:
				assert(0);
				break;
		}

		HC_Set_Camera_Position(cSetPosition.x, cSetPosition.y, cSetPosition.z);
		HC_Set_Camera_Up_Vector(cSetUpVector.x, cSetUpVector.y, cSetUpVector.z);

		HC_Show_Net_Camera(&cn, &tn, &un, &widthn, &heightn, lprojection);

	} HC_Close_Segment();


	if (GetSmoothTransition()) {
		HUtility::SmoothTransition(cPosition, cTarget, cUpVector, widtho, heighto, cn, tn, un, widthn, heightn, this);
	}
	else {
		if (GetModel()->GetContainsDouble()) {
			HC_Convert_Precision(GetSceneKey(), "double, camera");
		}
	}

	CameraPositionChanged(true, GetSmoothTransition());

	Update();
}

void H3DF::BaseView::SetNavigationCube(NavigationCube * pcNaviCube)
{
	m_pcNaviCube = pcNaviCube;
}

//== ViewPrivate Class =============================================================================

H3DF::ViewImpl::ViewImpl()
{
}

H3DF::ViewImpl::~ViewImpl()
{
	if (nullptr != m_pchName) {
		delete[] m_pchName;
	}
}

void H3DF::ViewImpl::Copy(const ViewImpl * pcInThat)
{
	m_pcBaseView = pcInThat->m_pcBaseView;
	m_pcWindow = pcInThat->m_pcWindow;

	if (nullptr != pcInThat->m_pchName) {

		// 입력 문자열의 크기 계산
		size_t nSourceSize = strlen(pcInThat->m_pchName) + 1; // 널 종료 문자('\0')를 포함해서 크기 계산

		// 대상 문자열에 충분한 메모리 할당
		m_pchName = new char[nSourceSize * sizeof(char)];
		if (nullptr == m_pchName) {
			// 메모리 할당 실패 처리
			
		}

		// 문자열 복사
		strcpy(m_pchName, pcInThat->m_pchName);

		//H3DF::Utility::CopyString(pcInThat->m_pchName, m_pchName);
	}

	m_bShowCollisions = pcInThat->m_bShowCollisions;

	m_pcCameraSelect = pcInThat->m_pcCameraSelect;
	m_pcSelectArea = pcInThat->m_pcSelectArea;

	if (nullptr != m_pcBaseView) {
		m_pcBaseView->SetNavigationCube(&m_cNaviCube);
	}
}

// 1. BaseView를 초기화 하는 부분
bool H3DF::ViewImpl::Init(H3DF::Model * pcInModel, const char * pchInDriverType, const char * pchInInstanceName, H3DF::WindowHandle nInWindowHandle)
{
	ModelImpl * pcModelImpl = static_cast<ModelImpl *>(pcInModel->GetImpl());
	DEBUG_VALID(pcModelImpl);

	// HBaseView 생성
	m_pcBaseView = new H3DF::BaseView((HBaseModel *)pcModelImpl,
		nullptr,											// Alias
		pchInDriverType,									// Driver Type
		pchInInstanceName,									// Instance name
		reinterpret_cast<void *>(nInWindowHandle),			// Window handle
		nullptr);

	if (nullptr == m_pcBaseView) {
		return false;
	}

	m_pcBaseView->Init();

	// View Segment Key 설정.View Segment에는 향후 사용하기 위한 Base View 정보를 추가해놓는다.
	m_cKey.Set(m_pcBaseView->GetViewKey());
	SegmentKeyImpl * pcKeyImpl = static_cast<SegmentKeyImpl *>(m_cKey.GetImpl());
	DEBUG_VALID(pcKeyImpl);
	pcKeyImpl->SetBaseView(m_pcBaseView);

	H3DF::SelectionSet * pcSelection = new H3DF::SelectionSet(m_pcBaseView);
	m_pcBaseView->SetSelection(pcSelection);

	// Model 설정
	m_pcModel = pcInModel;
	m_cModelKey = m_pcModel->GetSegmentKey();

	HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
		HC_Set_Selectability("everything = off");
	} HC_Close_Segment();

	// set up some scene defaults
	HC_Open_Segment_By_Key(m_pcBaseView->GetSceneKey()); {
		// #Selection: Line이 더 잘보이게 하고 선택이 잘되도록 하기 위해서 Face를 뒤로 보냄
		//HC_Set_Rendering_Options("face displacement = 16"); // 양수값이 Camera에서 멀어지는 방향임.
		HC_Set_Rendering_Options("face displacement = 2"); // 양수값이 Camera에서 멀어지는 방향임.
		HC_Set_Rendering_Options("no color interpolation, color index interpolation");
		HC_Set_Visibility("lights = (faces = on, edges = off), markers = off, faces=on, edges=off, lines=on, text = on");
	} HC_Close_Segment();

	SegmentKey cSceneKey(m_pcBaseView->GetSceneKey());
	cSceneKey.GetMaterialMappingControl().SetEdgeColor(RGBAColor(0, 0, 0));

	// windowspace (overlay) defaults
	HC_Open_Segment_By_Key(m_pcBaseView->GetWindowspaceKey()); {
		HC_Set_Color_By_Index("geometry", 3);
		HC_Set_Color_By_Index("window contrast", 1);
		HC_Set_Color_By_Index("windows", 1);
		HC_Set_Visibility("markers=on");
		HC_Set_Marker_Symbol("+");
		HC_Set_Selectability("off");
	} HC_Close_Segment();

	// GPU 설정, Default값을 사용, Driver는 View가 생성될 때 설정함.
	SetGpu(TheKenel.General.Display.Gpu);

	// do all the setup with no updates
	m_pcBaseView->SetSuppressUpdate(true);

	char chDriverOpts[MVO_BUFFER_SIZE], chRenderingOpts[MVO_BUFFER_SIZE] = { 0 };
	
	SetDriverOption();

	HC_Open_Segment_By_Key(m_pcBaseView->GetConstructionKey()); {
		// 		if (true == TheKenel.Appearance.AntiAliasing.Use) {
		// 			// Rendering Option에서는 Screen On만 설정한다.
		// 			HC_Set_Rendering_Options("anti-alias = (screen = on)");
		// 		}
	} HC_Close_Segment();

	if (false == TheKenel.Lighting.Light.Scaling) {
		m_pcBaseView->SetLightScaling(0);
	}
	else {
		m_pcBaseView->SetLightScaling(TheKenel.Lighting.Light.ScaleFactor / 100000.f);
	}

	m_pcBaseView->SetLightFollowsCamera(TheKenel.Lighting.Light.FollowsCamera);
	//SetLightCount(LightCount); //defer until after camera is all set up
	// SetDeepSelectionMode(DeepSelection); OCC를 사용할 때 대응하는 함수

	m_pcBaseView->SetVisibilitySelectionMode(TheKenel.Selection.Behavior.VisibilitySelection);
	m_pcBaseView->SetDynamicHighlighting(TheKenel.Selection.Behavior.DynamicHighlighting);
	m_pcBaseView->SetDetailSelection(TheKenel.Selection.Behavior.DetailSelection); // "Honor Line/Edge Weight/Pattern"
	m_pcBaseView->SetRelatedSelectionLimit(TheKenel.Selection.Behavior.RelatedSelectionLimit);
	m_pcBaseView->SetTransparentSelectionBoxMode(TheKenel.Selection.Behavior.UseSelectBox); // show a transparent box when selecting areas
	m_pcBaseView->SetRespectSelectionCulling(TheKenel.Selection.Behavior.RespectCulling); // Respect Culling during selection.
	m_pcBaseView->SetFastFitWorld(true);
	m_pcBaseView->SetForceFastHiddenLine(TheKenel.Performance.Optimization.HiddenLineMode == FastHiddenLine);
	m_pcBaseView->SetSpritingMode(TheKenel.Interaction.GeometryManipulation.Spriting);
	m_pcBaseView->SetAllowInteractiveCutGeometry(TheKenel.Interaction.GeometryManipulation.UpdateCutGeometry);
	m_pcBaseView->SetAllowInteractiveShadows(TheKenel.Interaction.GeometryManipulation.UpdateShadows);
	m_pcBaseView->SetBackplaneCulling(TheKenel.General.Etc.BackplaneCulling);
	m_pcBaseView->SetDisplayListType(DisplayListSegment);// DisplayListOff);

	SetupFrameRateMode();

	m_pcBaseView->SetSmoothTransition(false);

	m_pcBaseView->SetShadowRenderingMode((HShadowRenderingMode)TheKenel.Effects.SimpleShadow.ShadowRenderingMode);

	SetViewAxis();

	SetTransparency();

	//m_pcBaseView->SetAxisMode(TheKenel.General.Rendering.DisplayAxisTriad ? AxisOn : AxisOff);

	// 배경화면 설정
	SetWindowBackGroundColor(TheKenel.Appearance.BackgroundColor.Top, TheKenel.Appearance.BackgroundColor.Bottom);

	HPoint FakeHLRColor;
	FakeHLRColor.Set(ColorValue(ThePreset.FakeHLRColor));

	m_pcBaseView->SetFakeHLRColor(FakeHLRColor);
	m_pcBaseView->SetProjMode((ProjMode)ThePreset.ProjectionMode);
	m_pcBaseView->SetSmoothTransition(ThePreset.SmoothTransition);
	m_pcBaseView->SetSmoothTransitionDuration(0.5f);
	m_pcBaseView->GetUndoManager()->Flush();			//don't care about this initial camera change
	m_pcBaseView->SetDisplayHandlesOnDblClk(!ThePreset.DisableEditing);

	//SetCoordinateSystemHandedness(bWorldHandedness ? HandednessRight : HandednessLeft, true);
	m_pcBaseView->SetHandedness(ThePreset.WorldHandedness ? HandednessRight : HandednessLeft, true);

	// The state of world today with polygon handedness is
	// 1. Since we are using display lists by default, we want this setting.
	// 2. We will have it only on the view key. If required, model could have it's own
	// Rajesh B (11-Apr-2003)
	m_pcBaseView->SetPolygonHandednessMode(HandednessLeft);

	HPixelRGBA cHighlightSelectColor;
	cHighlightSelectColor.Set(255, 0, 0);

	m_pcBaseView->GetHighlightSelection()->SetSelectionFaceColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionEdgeColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionMarkerColor(cHighlightSelectColor);

	// #Selection: Highlighting Line, Edge 두께 설정

	// 아래 부분을 삭제하면 다음에 설정된 fLineWeight를 적용할 때 Segment 오류가 발생함.
	m_pcBaseView->GetSelection()->SetSelectionEdgeWeight(1.0);
	m_pcBaseView->GetHighlightSelection()->SetSelectionEdgeWeight(1.0);

	float fLineWeight = 2.0;
	HC_KEY nHighlightSelectionKey = m_pcBaseView->GetHighlightSelection()->GetSelectionSegment();
	HC_Open_Segment_By_Key(nHighlightSelectionKey); {
		HC_Set_Line_Weight(fLineWeight);
		HC_Set_Edge_Weight(fLineWeight);
	} HC_Close_Segment();

	HC_KEY nSelectionKey = m_pcBaseView->GetSelection()->GetSelectionSegment();
	HC_Open_Segment_By_Key(nSelectionKey); {
		HC_Set_Line_Weight(fLineWeight);
		HC_Set_Edge_Weight(fLineWeight);
	} HC_Close_Segment();

	m_pcBaseView->GetHighlightSelection()->SetGrayScale(false);// ThePreset.GrayScaleSelection);
	m_pcBaseView->GetHighlightSelection()->SetUseDefinedHighlight(false);// ThePreset.UseDefinedHighlighting);
	m_pcBaseView->GetHighlightSelection()->SetAllowDisplacement(false);// ThePreset.DisplaceSelection);
	m_pcBaseView->GetHighlightSelection()->UpdateHighlightStyle();

	// set the selection color
	HSelectionSet * sel_set = m_pcBaseView->GetSelection();
	assert(sel_set);
	HPixelRGBA cSelectColor;
	int sel_alpha = (int)(ThePreset.SelectionColorTransparency * 2.56f);		// settings is a %, scale it to 256
	cSelectColor.Set(ColorRGBA(ThePreset.PolygonSelectionColor, sel_alpha));
	sel_set->SetSelectionFaceColor(cSelectColor);

	cSelectColor.Set(ColorRGBA(ThePreset.LineSelectionColor, sel_alpha));
	sel_set->SetSelectionEdgeColor(cSelectColor);

	cSelectColor.Set(ColorRGBA(ThePreset.MarkerSelectionColor, sel_alpha));
	sel_set->SetSelectionMarkerColor(cSelectColor);

	// set markup color and weight
	SetMarkupColor(ThePreset.MarkupColor);

	SetShadowColor(TheKenel.VisualEffects.Shadow.GetColor());

	m_pcBaseView->GetMarkupManager()->SetMarkupWeight(ThePreset.MarkupWeight / 100.0f);
	// 	m_pcBaseView->SetShadowResolution(TheKenel.VisualEffects.Shadow.GetResolution());
	// 	m_pcBaseView->SetShadowBlurring(TheKenel.VisualEffects.Shadow.Blurring);

		// set the color index interpolation settings
	m_pcBaseView->SetColorInterpolation(ThePreset.CiByValue);
	m_pcBaseView->SetColorIndexInterpolation(ThePreset.CiByColormapIndex, ThePreset.CiIsolines);

	m_pcBaseView->GetSelection()->SetGrayScale(ThePreset.GrayScaleSelection);
	m_pcBaseView->GetSelection()->SetUseDefinedHighlight(ThePreset.UseDefinedHighlighting);
	m_pcBaseView->GetSelection()->SetAllowDisplacement(ThePreset.DisplaceSelection);
	m_pcBaseView->GetSelection()->SetHighlightMode(HighlightQuickmoves);
	m_pcBaseView->GetHighlightSelection()->SetHighlightMode(HighlightQuickmoves);

	m_pcBaseView->GetSelection()->SetHighlightTransparency(ThePreset.TransparencyLevel);

	if (ThePreset.RefSelType == "Spriting") {
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelSpriting);
	}
	else if (ThePreset.RefSelType == "Off") {
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelOff);
	}
	else {
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelDefault);
	}

	m_pcBaseView->GetHighlightSelection()->UpdateHighlightStyle();
	m_pcBaseView->GetSelection()->UpdateHighlightStyle();

	// set the rendermode
	m_pcBaseView->SetRenderMode((HRenderMode)ThePreset.RenderMode, true);

	m_pcBaseView->SetEventCheckerCallback(event_checker);

	char chRenderingOption[MVO_BUFFER_SIZE] = "0";
	char chHeuristics[MVO_BUFFER_SIZE] = "0";
	char chNetHeuristics[MVO_BUFFER_SIZE] = "0";

	/*
		HC_KEY nHighlightStyleKey = m_pcBaseView->GetHighlightSelection()->GetHighlightStyle();
		HC_Open_Segment_By_Key(nHighlightStyleKey); {
			HC_Show_Rendering_Options(chRenderingOption);
			HC_Show_Heuristics(chHeuristics);
		} HC_Close_Segment();

		HC_KEY nSelectionSegmentKey = m_pcBaseView->GetHighlightSelection()->GetSelectionSegment();
		HC_Open_Segment_By_Key(nSelectionSegmentKey); {
			HC_Show_Rendering_Options(chRenderingOption);
			HC_Show_Heuristics(chHeuristics);
		} HC_Close_Segment();

		HC_KEY nSceneKey = m_pcBaseView->GetSceneKey();
		HC_Open_Segment_By_Key(nSceneKey); {
			HC_Open_Segment("./overwrite/lights/selection_segment"); {
				HC_Show_Net_Heuristics(chNetHeuristics);
			}HC_Close_Segment();

			HC_Show_Rendering_Options(chRenderingOption);
			HC_Show_Heuristics(chHeuristics);
		} HC_Close_Segment();
	*/

	SetShowCollisions(ThePreset.ShowCollisions);

	SetSceneFont(ThePreset.FontName, ThePreset.FontSize, ThePreset.FontUnits);

	HC_Open_Segment_By_Key(m_pcBaseView->GetCuttingPlanesKey()); {
		HC_Open_Segment("plane2"); {
			HC_Rotate_Object(90, 0, 0);
		}HC_Close_Segment();

		HC_Open_Segment("plane3"); {
			HC_Rotate_Object(0, 0, 90);
		}HC_Close_Segment();
	}HC_Close_Segment();

	//apply hiding of overlapped text (or not)
	m_pcBaseView->SetHideOverlappedText(ThePreset.HideOverlappedText);

	/*
		HC_Open_Segment_By_Key(m_pcBaseView->GetShadowMapSegmentKey()); {
			if (ThePreset.ShadowMap) {
				sprintf(chRenderingOpts, "shadow map=(on, resolution=%d, samples=%d, %s jitter, %s)",
					ThePreset.SMResolution, ThePreset.SMSamples,
					(ThePreset.Jitter ? "" : "no"),
					(ThePreset.ViewDependentShadowMap ? "view dependent" : "view independent"));
			}
			else {
				sprintf(chRenderingOpts, "no shadow map");
			}
			HC_Set_Rendering_Options(chRenderingOpts);
		} HC_Close_Segment();
	*/

	HC_Open_Segment_By_Key(m_pcBaseView->GetSceneKey()); {
		HC_Set_Variable_Edge_Weight(Utility::ToChar(ThePreset.LineWeight));
		HC_Set_Variable_Line_Weight(Utility::ToChar(ThePreset.LineWeight));

		//apply stereo mode
		if (ThePreset.StereoMode) {
			HCLOCALE(sprintf(chRenderingOpts, "stereo, stereo separation = %f", ThePreset.StereoSeparation / 10000.f));
			HC_Set_Rendering_Options(chRenderingOpts);
		}

		HCLOCALE(sprintf(chRenderingOpts, "simple shadow = (opacity = %f)", TheKenel.VisualEffects.Shadow.GetOpacity()));
		HC_Set_Rendering_Options(chRenderingOpts);

		char gooch_color_map[4096];
		HCLOCALE(sprintf(gooch_color_map, "(R=%f G=%f B=%f), (R=%f G=%f B=%f), (R=%f G=%f B=%f), (R=%f G=%f B=%f), (R=%f G=%f B=%f), (R=%f G=%f B=%f)",
			ColorValue(ThePreset.GoochColor1),
			ColorValue(ThePreset.GoochColor2),
			ColorValue(ThePreset.GoochColor3),
			ColorValue(ThePreset.GoochColor4),
			ColorValue(ThePreset.GoochColor5),
			ColorValue(ThePreset.GoochColor6))
		);
		ThePreset.GoochColorMap = gooch_color_map;

		HC_Open_Segment("./overwrite/lights/gooch_color_map_segment"); {
			HC_Set_Color_Map(Utility::ToChar(ThePreset.GoochColorMap));
		}HC_Close_Segment();

		HCLOCALE(sprintf(chRenderingOpts, "gooch options = (diffuse weight = %f, color range=(0.0, %f), color map segment = `./overwrite/lights/gooch_color_map_segment`)",
			ThePreset.GoochWeight, ThePreset.GoochColorHigh));
		HC_Set_Rendering_Options(chRenderingOpts);

		//set camera near limit
		HC_Set_Camera_Near_Limit(ThePreset.NearCameraLimit / 100000.f);

		//Apply curve geometry options
		char curve_opt[4096];
		HCLOCALE(sprintf(curve_opt, "general curve = (budget = %d, continued budget = %d, maximum deviation = %f, maximum angle = %f, maximum length = %f, %s view independent)",
			ThePreset.Budget, ThePreset.ContinuedBudget, ThePreset.MaxDeviation / 10000.f,
			ThePreset.MaxAngle / 10000.f, ThePreset.MaxLength / 10000.f, ThePreset.ViewIndependent ? "" : "no"));

		HC_Set_Rendering_Options(curve_opt);

		// 		m_pcBaseView->SetReflectionPlane(ThePreset.ReflectionPlane, ThePreset.ReflectionOpacity,
		// 			ThePreset.ReflectionFading, ThePreset.ReflectionUseAttenuation,
		// 			ThePreset.ReflectionHither, ThePreset.ReflectionYon,
		// 			ThePreset.ReflectionUseBlur, ThePreset.ReflectionBlur);

		char ambient_color[MVO_BUFFER_SIZE];
		if (ThePreset.HemisphericAmbient)
		{
			float r1, g1, b1, r2, g2, b2;
			char ropt[MVO_BUFFER_SIZE];

			//darken all colors by about 75%
			r1 = (GetRValue(ThePreset.AmbientTopColor) >> 2) / 255.0f;
			g1 = (GetGValue(ThePreset.AmbientTopColor) >> 2) / 255.0f;
			b1 = (GetBValue(ThePreset.AmbientTopColor) >> 2) / 255.0f;

			r2 = (GetRValue(ThePreset.AmbientBottomColor) >> 2) / 255.0f;
			g2 = (GetGValue(ThePreset.AmbientBottomColor) >> 2) / 255.0f;
			b2 = (GetBValue(ThePreset.AmbientBottomColor) >> 2) / 255.0f;

			sprintf(ambient_color, "ambient up=(R=%f G=%f B=%f), ambient down=(R=%f G=%f B=%f)",
				r1, g1, b1, r2, g2, b2);

			HC_Set_Color(ambient_color);
			HC_Define_System_Options("disable ambient material");

			if (ThePreset.UseAmbientUpVector) {
				HCLOCALE(sprintf(ropt, "ambient up vector = (%f, %f, %f)",
					ThePreset.AmbientUpVector.x, ThePreset.AmbientUpVector.y,
					ThePreset.AmbientUpVector.z));
				HC_Set_Rendering_Options(ropt);
			}
			else
			{
				HC_Set_Rendering_Options("no ambient up vector");
			}
		}
		else
		{
			float r1, g1, b1;

			//darken all colors by about 75%
			r1 = (GetRValue(ThePreset.AmbientTopColor) >> 2) / 255.0f;
			g1 = (GetGValue(ThePreset.AmbientTopColor) >> 2) / 255.0f;
			b1 = (GetBValue(ThePreset.AmbientTopColor) >> 2) / 255.0f;

			HCLOCALE(sprintf(ambient_color, "ambient=(R=%f G=%f B=%f)", r1, g1, b1));
			HC_Set_Color(ambient_color);
			HC_Set_Rendering_Options("no ambient up vector");
			HC_Define_System_Options("no disable ambient material");
		}

		//Apply Greeking Settings
		char cGreekingSettings[2048] = "no greeking limit";
		if (ThePreset.UseGreeking)
		{
			CString csGreekingSettings;
			csGreekingSettings.Format(_T("greeking mode= %s, greeking limit= %f %s"),
				ThePreset.GreekingMode, ThePreset.GreekingLimit / 1000.f,
				ThePreset.GreekingUnits);
			strcpy(cGreekingSettings, Utility::ToChar(csGreekingSettings));
		}
		HC_Set_Text_Font(cGreekingSettings);
	} HC_Close_Segment();

	/*  // Remark
		if (CAppSettings::ChildStartMaximized)
		{
			WINDOWPLACEMENT wp;
			this->GetParent()->GetWindowPlacement(&wp);
			wp.showCmd = SW_SHOWMAXIMIZED;
			this->GetParent()->SetWindowPlacement(&wp);
		}
	*/

	m_pcBaseView->SetLightCount(ThePreset.LightCount);
	m_pcBaseView->SetViewSelectionLevel(HSelectionLevelSegment);

	// 메모리 소모가 많고 속도에는 큰 도움이 되지 않으므로 사용하지 않는다.
	m_pcBaseView->GetModel()->SetStaticModel(false);
	m_pcBaseView->GetModel()->SetLMVModel(false);

	// WindowKey 선언 위치가 변경되면 않됨. 주의할것.
	m_pcWindow = new WindowKey(m_pcBaseView);

	m_cNaviCube.SetView(m_pcBaseView, m_pcWindow);
	m_pcBaseView->SetNavigationCube(&m_cNaviCube);

	SetDefaultOperator();

	// Object Snap용 Glyph 생성
	//Operator::ObjectSnap::CreateGlyph();

	SetSelectOption();

	// Portfolio Key 생성
// 	SegmentKey cPortfoliosKey = m_cKey.Subsegment(L"Portfolios");
// 	m_cPortfolioKey.SetKeyValue(cPortfoliosKey.KeyValue());

	m_cPortfolioKey = Database::CreatePortfolio();

	// do all the setup with no updates
	m_pcBaseView->SetSuppressUpdate(false);

	return true;
}

// 1.1 Gpu 설정
void H3DF::ViewImpl::SetGpu(CString strGpu)
{
	char gpu_to_use[256];
	strcpy(gpu_to_use, (char const *)H_UTF8(strGpu).encodedText());
	if (strcmp(gpu_to_use, "Default") != 0)
	{
		HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
			HC_Set_Driver_Options(H_FORMAT_TEXT("gpu preference = specific = %s", gpu_to_use));
		} HC_Close_Segment();
	}
}

// 1.2 Driver Option 설정
void H3DF::ViewImpl::SetDriverOption()
{
	char chDriverOpts[MVO_BUFFER_SIZE] = { 0 };

	sprintf(chDriverOpts, "quick moves preference = %s", Utility::ToChar(TheKenel.Selection.Highlight.QuickMovesType).GetBuffer());

	if (true == TheKenel.General.Display.DriverDisplayStats) {
		sprintf(chDriverOpts, "%s, display stats, display time stats, display memory stats", chDriverOpts);
	}

	if (true == TheKenel.General.Display.StereoMode) {
		sprintf(chDriverOpts, "%s, stereo", chDriverOpts);
	}

	HCLOCALE(sprintf(chDriverOpts,
		"%s, ambient occlusion = (%s, strength = %f, quality = %s), fast silhouette edges = (%s, tolerance = %f, %s heavy exterior)", chDriverOpts,
		(TheKenel.Effects.FrameBuffer.UseAmbient ? "on" : "off"), TheKenel.Effects.FrameBuffer.AmbientStrength,
		(TheKenel.Effects.FrameBuffer.HighQualityAmbient ? "nicest" : "fast"),
		(TheKenel.Effects.FrameBuffer.UseFastSilhouette ? "on" : "off"), TheKenel.Effects.FrameBuffer.FastSilhouetteTolerance,
		(TheKenel.Effects.FrameBuffer.HeavyExteriorSilhouette ? "" : "no")));

	m_pcBaseView->SetDoubleBuffering(TheKenel.General.Display.DoubleBuffer);

	HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
		HC_Set_User_Index(H_VIEW_POINTER_INDEX, GetBaseView());  // This is used in the event_checker for constant framerate.

		HC_Set_Driver_Options(chDriverOpts);
		HC_Set_Driver_Options("special events, update interrupts");
		HC_Control_Update(".", "redraw everything");
	} HC_Close_Segment();

	SetAntiAliasOption();
}

// 1.2.1 Anti Alias Option 설정
void H3DF::ViewImpl::SetAntiAliasOption()
{
	char chDriverOpts[MVO_BUFFER_SIZE] = { 0 };

	// set anti-aliasing if set
	if (true == TheKenel.Appearance.AntiAliasing.Use) {
		sprintf(chDriverOpts, "anti-alias = %d", TheKenel.Appearance.AntiAliasing.Level);

		HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
			HC_Set_Driver_Options(chDriverOpts);
			// Rendering Option에서는 Screen On만 설정한다.
			HC_Set_Rendering_Options("anti-alias = (screen = on)");
		} HC_Close_Segment();

		GetBaseView()->SetTextAntialiasing(TheKenel.Appearance.AntiAliasing.Text);

		// Line에 대해서는 AntiAliasing을 적용하지 않는다. 적용하지 않아도, Line에 AntiAliasing이 적용됨.
		// Line에 적용할 경우 Transparency가 적용된 Face의 색상이 Line에 잔상처럼 나타나는 문제가 있음.
		GetBaseView()->SetLineAntialiasing(false);
	}
}

// 1.3 투명도 적용 방법 설정
void H3DF::ViewImpl::SetTransparency()
{
	char chText[4096];
	char chSorting[4096];
	bool bFastZsort = false;

	strcpy(chSorting, Utility::ToChar(TheKenel.General.Transparency.Sorting));

	if (strstr(chSorting, "z-sort")) {
		if (strstr(chSorting, "fast")) {
			bFastZsort = true;
		}
		sprintf(chSorting, "z-sort only");
	}

	sprintf(chText, "style = %s, hsr algorithm = %s, depth peeling options = (layers= %s, algorithm=%s), depth writing = %s",
		Utility::ToChar(TheKenel.General.Transparency.Style),
		chSorting,
		Utility::ToChar(TheKenel.General.Transparency.DepthPeelingLayers),
		TheKenel.General.Transparency.PixelOIT ? "pixel" : "buffer",
		TheKenel.General.Transparency.DepthWriting == true ? "on" : "off");

	m_pcBaseView->SetTransparency(chText, bFastZsort);
}

void H3DF::ViewImpl::SetupFrameRateMode()
{
	if (true == TheKenel.Performance.FramerateOptimization.UseFramerate)
	{
		m_pcBaseView->SetFramerateMode((FramerateMode)TheKenel.Performance.FramerateOptimization.CurrentFramerateMode,
			TheKenel.Performance.FramerateOptimization.FramerateTime, TheKenel.Performance.FramerateOptimization.MaxThreshold,
			UINT2bool(TheKenel.Performance.FramerateOptimization.UseLods), TheKenel.Performance.FramerateOptimization.DetailSteps,
			TheKenel.Performance.FramerateOptimization.HardCutoff);
	}
	else if (TheKenel.Performance.FramerateOptimization.CullingThresholdSet)
	{
		m_pcBaseView->SetFramerateMode(FramerateOff);
		m_pcBaseView->SetCullingThreshold(TheKenel.Performance.FramerateOptimization.CullingThreshold);
	}
	else
	{
		m_pcBaseView->SetFramerateMode(FramerateOff);
		m_pcBaseView->SetCullingThreshold(0);
	}
}

bool H3DF::ViewImpl::IsInitNavigationCube()
{
	return m_cNaviCube.IsInitialized();
}

void H3DF::ViewImpl::InitNavigationCube(int nWidth, int nHeight)
{
	//m_cNaviCube.SetView(m_pcBaseView, m_pcWindow);
	m_cNaviCube.Create(nWidth, nHeight, m_pcBaseView->GetModelKey());
	m_cNaviCube.Transform();

	//m_pcBaseView->SetNavigationCube(&m_cNaviCube);
}

void H3DF::ViewImpl::Resize(int x, int y)
{
	GetBaseView()->SetXYSizeOverride(x, y);

	if (x > 0 && y > 0 && true == IsInitNavigationCube()) {
		m_cNaviCube.OnSize(x, y);
	}
}

bool H3DF::ViewImpl::GetKeyState(unsigned int key, int & flags)
{
	unsigned char state[256];
	flags = 0;
	GetKeyboardState(state);
	if (state[VK_LSHIFT] >= 128)
		flags |= MVO_LEFT_SHIFT;
	if (state[VK_SHIFT] >= 128)
		flags |= MVO_SHIFT;
	if (state[VK_RSHIFT] >= 128)
		flags |= MVO_RIGHT_SHIFT;
	if (state[VK_CONTROL] >= 128)
		flags |= MVO_CONTROL;
	if (state[VK_LMENU] >= 128 || state[VK_RMENU] >= 128)
		flags |= MVO_ALT;
	if (state[key] >= 128)
		return true;
	else
		return false;
}

void H3DF::ViewImpl::SetMarkupColor(COLORREF new_color, bool emit_message)
{
	UNREFERENCED(emit_message);

	HPoint new_mkp_color;
	new_mkp_color.Set(ColorValue(new_color));

	HC_Open_Segment_By_Key(m_pcBaseView->GetMarkupManager()->GetMarkupKey()); {
		HC_Set_Color_By_Value("everything", "RGB", new_mkp_color.x, new_mkp_color.y, new_mkp_color.z);
	}HC_Close_Segment();

	m_pcBaseView->GetMarkupManager()->SetMarkupColor(new_mkp_color);
}

void H3DF::ViewImpl::SetShadowColor(RGBAColor cInColor)
{
	HPoint new_shd_color;
	new_shd_color.Set(cInColor.red, cInColor.green, cInColor.blue);
	m_pcBaseView->SetShadowColor(new_shd_color);
}

void H3DF::ViewImpl::event_checker(HIC_Rendition const * nr)
{
	//MSG msg;
	H3DF::BaseView * pCurrentView = (H3DF::BaseView *)HIC_Show_User_Index(nr, H_VIEW_POINTER_INDEX);

	if (pCurrentView)
	{
		int state = GetAsyncKeyState(VK_LBUTTON);
		if (state & 32768)
		{
			pCurrentView->GetConstantFrameRateObject()->InitiateDelay();

			pCurrentView->SetUpdateInterrupted(true);

			HIC_Abort_Update(nr);
		}

		state = GetAsyncKeyState(VK_MBUTTON);
		if (state & 32768)
		{
			HIC_Abort_Update(nr);
			pCurrentView->SetUpdateInterrupted(true);
		}

// 		if (PeekMessage(&msg, pCurrentView->m_hWnd, WM_MOUSEWHEEL, WM_MOUSEWHEEL, PM_NOREMOVE))
// 		{
// 			HIC_Abort_Update(nr);
// 			SetUpdateInterrupted(true);
// 		}
	}
	int state = GetAsyncKeyState(VK_RBUTTON);
	if (state & 32768)
	{
		pCurrentView->GetConstantFrameRateObject()->InitiateDelay();
		pCurrentView->SetUpdateInterrupted(2);
		HIC_Abort_Update(nr);
	}

}

void H3DF::ViewImpl::SetViewAxis()
{
/*
	char text[4096];
	HVector front, top;
	CString strViewAxis = "1  0  0  0  1  0  0  0  1";
	strcpy(text, Utility::ToChar(strViewAxis));
	sscanf(text, "%f %f %f %f %f %f", 
		front.x, &front.y, &front.z, &top.x, &top.y, &top.z);
*/
	HVector front(1, 0, 0), top(0, 1, 0);
	m_pcBaseView->SetViewAxis(&front, &top);
}

// Select option 처리
void H3DF::ViewImpl::SetSelectOption()
{
	MaterialMappingKit cMaterial;
	cMaterial.SetFaceColor(RGBColor(1.0f, 0.5f, 0.0f));
	cMaterial.SetEdgeColor(RGBColor(1.0f, 0.5f, 0.0f));
	cMaterial.SetLineColor(RGBColor(1.0f, 0.5f, 0.0f));

	// #Selection: Selection Option 설정 
	m_pcWindow->GetSelectionOptionsControl().SetLevel(Selection::Level::Entity);
	//m_pcWindow->GetSelectionOptionsControl().SetRelatedLimit(10);
	//m_pcWindow->GetSelectionOptionsControl().SetProximity(0.05f);
	//m_pcWindow->GetSelectionOptionsControl().SetBias(Selection::Bias::Lines);
	//m_pcWindow->GetSelectionOptionsControl().SetSorting(Selection::Sorting::Proximity); // Sorting 해도 Z방향 Sort가 정확하게 되지는 않됨.

	m_pcWindow->GetHighlightControl().SetMaterialMapping(cMaterial);
	m_pcWindow->GetHighlightControl().GetLineAttributeControl().SetWeight(5.0);

	return;
}


void H3DF::ViewImpl::SetWindowBackGroundColor(COLORREF nNewTopColor, COLORREF nNewBottomColor, bool bEmitMessage)
{
	HPoint nWindowTopColor;
	nWindowTopColor.Set(ColorValue(nNewTopColor));

	HPoint nWindowBottomColor;
	nWindowBottomColor.Set(ColorValue(nNewBottomColor));

	m_pcBaseView->SetWindowColor(nWindowTopColor, nWindowBottomColor, bEmitMessage);
}

void H3DF::ViewImpl::SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits)
{
	HC_Open_Segment_By_Key(GetBaseView()->GetSceneKey()); {
		// first let's query the user's font size settings
		float size = (float)atof(Utility::ToChar(csFontSize));
		if (size < 0) {
			size *= -1;
		}

		char cfname[MVO_BUFFER_SIZE];
		sprintf(cfname, "name = \"%s\"", (const char *)Utility::ToChar(csFontName));
		HC_Set_Text_Font(cfname);

		// set the font size via MVO - to propogate it to the hnet clients
		// hnet removed: do we still need to do this?
		char cfsize[MVO_BUFFER_SIZE];
		HCLOCALE(sprintf(cfsize, "%f %s", size, (const char *)Utility::ToChar(csFontUnits)));

		GetBaseView()->SetFontSize(cfsize, true);

	} HC_Close_Segment();
}

//== Operator 관련 함수 ==============================================================================

void H3DF::ViewImpl::SetDefaultOperator()
{
	//m_pcCameraManipulate = new HOpCameraManipulate(this, 0, 1, new OpCameraOrbitSelect(this), new OpCameraPan(this));
// 		, new HSOpCameraPan(m_pHView),
// 		new HSOpCameraZoom(m_pHView), 0, false))

	m_pcCameraSelect = new Operator::CameraControl(*m_pcWindow, m_cNaviCube);
	m_pcSelectArea = new Operator::SelectArea(GetBaseView());

	//LocalSetOperator(m_pcCameraManipulate);
}

void H3DF::ViewImpl::LocalSetOperator(HBaseOperator * pcNewOperator)
{
	HBaseOperator * pcOperator = GetBaseView()->GetOperator();
	GetBaseView()->SetOperator(pcNewOperator);

	if (nullptr != pcOperator) {
		delete pcOperator;
	}
}

bool H3DF::ViewImpl::GetSimpleShadow()
{
	return m_bSimpleShadowFlag;
}

void H3DF::ViewImpl::SetSimpleShadow(bool bFlag)
{
	m_bSimpleShadowFlag = bFlag;
}

bool H3DF::ViewImpl::GetSimpleReflection()
{
	return m_bSimpleReflection;
}

void H3DF::ViewImpl::SetSimpleReflection(bool bFlag)
{
	m_bSimpleReflection = bFlag;
}

//== Keyboard 관련 함수 ==============================================================================
bool H3DF::ViewImpl::Char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return true;
}

void H3DF::ViewImpl::ViewReady()
{
	HBaseModel * pcModel = GetBaseView()->GetModel();

	// GetBaseView()->SetSuppressUpdate(true);

	//GetBaseView()->SetSplatRendering(BOOL2bool(ThePreset.SplatRendering));

	GetBaseView()->SetFastMarkerDrawing(ThePreset.FastMarkers);

	 // Temp Remark
// 	GetBaseView()->SetShadowLightDirection(ThePreset.UseLightVector, (HPoint *)&ThePreset.LightVector);
// 	GetBaseView()->SetShadowIgnoresTransparency(TheKenel.VisualEffects.Shadow.IgnoreTransparency);
// 	GetBaseView()->SetShadowMode((HShadowMode) TheKenel.VisualEffects.Shadow.Mode);

	// GetBaseView()->SetOcclusionCullingMode(ThePreset.OcclusionCulling);

	//Turn on static model and display lists last, and in that order
 	//pcModel->SetStaticModel(TheKenel.Performance.Optimization.StaticModel);
}

#include "../3DF/Operator.KeyboardTest.h"
//:TEMP
Operator::KeyboardTest * g_pOperator = nullptr;

bool H3DF::ViewImpl::KeyboardInput(Json::Object & cInObject)
{
	using namespace Signal;

	if (g_pOperator == nullptr) {
		g_pOperator = new Operator::KeyboardTest(this->m_pcWindow);
	}

	Signal::View::Action action = (Signal::View::Action)cInObject.GetInteger(SKW_ACTION);

	switch (action) {
		case Signal::View::Action::OnChar:
		case Signal::View::Action::OnKeyDown:
		case Signal::View::Action::OnKeyUp:
		case Signal::View::Action::OnInput:
			g_pOperator->OnKeyboard(cInObject);
			break;

		default:
			return false;
	}

	return true;
}

DWORD H3DF::ViewImpl::MouseMapFlags(DWORD nState)
{
	DWORD nFlag = 0;

	// map the mfc events state to MVO
	if (nState & MK_LBUTTON) nFlag |= MVO_LBUTTON;
	if (nState & MK_RBUTTON) nFlag |= MVO_RBUTTON;
	if (nState & MK_MBUTTON) nFlag |= MVO_MBUTTON;
	if (nState & MK_SHIFT) nFlag |= MVO_SHIFT;
	if (nState & MK_CONTROL) nFlag |= MVO_CONTROL;

	return nFlag;
}