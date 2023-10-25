#include "StdAfx.h"

#include "View.Private.h"

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

#include "../3DF/SelectionSet.h"

#include "../3DF/NavigationCube.h"
#include "../3DF/3DF.Utility.h"
#include "../3DF/Facility.AppOptions.h"

#include "../3DF/Operator.CameraSelect.h"
#include "../3DF/Operator.SelectArea.h"
#include "../3DF/Operator.ObjectSnap.h"

#include "../3DF/Window.h"
#include "../3DF/Selectability.h"
#include "../3DF/Visibility.h"
#include "../3DF/Material.h"
#include "../3DF/LineAttribute.h"

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
		m_pcNaviCube->Transform();
	}

	HBaseView::UpdateInternal(antialias, force_update);
}

void H3DF::BaseView::SetViewMode(H3DF::ViewMode eViewMode, bool bFitWorld)
{
	H3DF::ViewMode eOldViewMode = m_eViewMode;
	m_eViewMode = eViewMode;

	if (H3DF::ViewMode::Unknown == eViewMode) {
		return;
	}

	HPoint target, camera, view;
	float fLength;

	HPoint cPosition, cTarget, cUpVector;
	float widtho, heighto;
	char projection[MVO_BUFFER_SIZE];

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

			case H3DF::ViewMode::right: {
				cSetPosition.Set(target.x + cLenFrontAxis.x, target.y + cLenFrontAxis.y, target.z + cLenFrontAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(0, -cUpVector.z, cUpVector.y);
				}
			} break;

			case H3DF::ViewMode::left: {
				cSetPosition.Set(target.x - cLenFrontAxis.x, target.y + cLenFrontAxis.y, target.z + cLenFrontAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(0, cUpVector.z, -cUpVector.y);
				}
			} break;

			case H3DF::ViewMode::front: {
				cSetPosition.Set(target.x + cLenTopAxis.x, target.y - cLenTopAxis.y, target.z + cLenTopAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(-cUpVector.z, 0, cUpVector.x);
				}
			} break;

			case H3DF::ViewMode::back: {
				cSetPosition.Set(target.x + cLenTopAxis.x, target.y + cLenTopAxis.y, target.z + cLenTopAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(cUpVector.z, 0, -cUpVector.x);
				}
			} break;

			case H3DF::ViewMode::top: {
				cSetPosition.Set(target.x + cLenRightAxis.x, target.y + cLenRightAxis.y, target.z - cLenRightAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 1, 0);
				}
				else {
					cSetUpVector.Set(-cUpVector.y, cUpVector.x, 0);
				}
			} break;

			case H3DF::ViewMode::bottom: {
				cSetPosition.Set(target.x + cLenRightAxis.x, target.y + cLenRightAxis.y, target.z + cLenRightAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 1, 0);
				}
				else {
					cSetUpVector.Set(cUpVector.y, -cUpVector.x, 0);
				}
			} break;

			case H3DF::ViewMode::py_nz: { // Back - Bottom
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

			case H3DF::ViewMode::py_pz: { // Top - Back
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

			case H3DF::ViewMode::ny_pz: { // Top - Front
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

			case H3DF::ViewMode::ny_nz: { // Front - Bottom
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

			case H3DF::ViewMode::nx_nz: { // Left - Bottom
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

			case H3DF::ViewMode::nx_pz: { // top - Left
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

			case H3DF::ViewMode::px_pz: { // Top - Right
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

			case H3DF::ViewMode::px_nz: { // Right - Bottom
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

			case H3DF::ViewMode::nx_py: { // Back - Left
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

			case H3DF::ViewMode::px_py: { // Right - Back
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

			case H3DF::ViewMode::px_ny: { // Front - Right
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

			case H3DF::ViewMode::nx_ny: { // Left - Front
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

			case H3DF::ViewMode::nx_py_nz: { // bottom - left - back
				cSetPosition.Set(target.x - cVertexVector.x, target.y + cVertexVector.y, -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, 0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.z, -cUpVector.x, -cUpVector.y);
				}
			} break;

			case H3DF::ViewMode::nx_py_pz: { // top - back - left
				cSetPosition.Set(target.x - cVertexVector.x, target.y + cVertexVector.y, target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(-cUpVector.y, cUpVector.z, -cUpVector.x);
				}
			} break;

			case H3DF::ViewMode::nx_ny_pz: { // top - left - front
				cSetPosition.Set(target.x - cVertexVector.x, -(target.y + cVertexVector.y), target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, 0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(-cUpVector.z, cUpVector.x, -cUpVector.y);
				}
			} break;

			case H3DF::ViewMode::nx_ny_nz: { // bottom - front - left
				cSetPosition.Set(target.x - cVertexVector.x, -(target.y + cVertexVector.y), -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.y, cUpVector.z, cUpVector.x);
				}
			} break;

			case H3DF::ViewMode::px_py_pz: { // ISO top - right - back
				cSetPosition.Set(target.x + cVertexVector.x, target.y + cVertexVector.y, target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.z, cUpVector.x, cUpVector.y);
				}
			} break;

			case H3DF::ViewMode::px_py_nz: { // bottom - back - right
				cSetPosition.Set(target.x + cVertexVector.x, target.y + cVertexVector.y, -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, 0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.y, -cUpVector.z, -cUpVector.x);
				}

			} break;

			case ViewMode::px_ny_nz: { // bottom - right - front
				cSetPosition.Set(target.x + cVertexVector.x, -(target.y + cVertexVector.y), -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(-cUpVector.z, -cUpVector.x, cUpVector.y);
				}

			} break;

			case ViewMode::px_ny_pz: { // top - front - right
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

		if (GetSmoothTransition())
		{
			HPoint cn, tn, un;
			float widthn, heightn;
			char lprojection[MVO_BUFFER_SIZE];

			HC_Show_Net_Camera(&cn, &tn, &un, &widthn, &heightn, lprojection);

			HUtility::SmoothTransition(cPosition, cTarget, cUpVector, widtho, heighto, cn, tn, un, widthn, heightn, this);
		}
		else
		{
			if (GetModel()->GetContainsDouble()) {
				HC_Convert_Precision(GetSceneKey(), "double, camera");
			}
		}

		CameraPositionChanged(true, GetSmoothTransition());

		Update();

	} HC_Close_Segment();
}

void H3DF::BaseView::SetNavigationCube(NavigationCube * pcNaviCube)
{
	m_pcNaviCube = pcNaviCube;
}

//== ViewPrivate Class =============================================================================

H3DF::ViewPrivate::ViewPrivate()
{
}

H3DF::ViewPrivate::~ViewPrivate()
{
	if (nullptr != m_pchName) {
		delete[] m_pchName;
	}
}

void H3DF::ViewPrivate::Copy(const ViewPrivate * pcInThat)
{
	m_pcBaseView = pcInThat->m_pcBaseView;
	m_pcWindow = pcInThat->m_pcWindow;

	if (nullptr != pcInThat->m_pchName) {

		// 입력 문자열의 크기 계산
		size_t nSourceSize = strlen(pcInThat->m_pchName) + 1; // 널 종료 문자('\0')를 포함해서 크기 계산

		// 대상 문자열에 충분한 메모리 할당
		m_pchName = new char(nSourceSize * sizeof(char));
		if (nullptr == m_pchName) {
			// 메모리 할당 실패 처리
			
		}

		// 문자열 복사
		strcpy(m_pchName, pcInThat->m_pchName);

		//H3DF::Utility::CopyString(pcInThat->m_pchName, m_pchName);
	}

	m_bShowCollisions = pcInThat->m_bShowCollisions;

	m_pcCameraOrbitSelect = pcInThat->m_pcCameraOrbitSelect;
	m_pcSelectArea = pcInThat->m_pcSelectArea;

	if (nullptr != m_pcBaseView) {
		m_pcBaseView->SetNavigationCube(&m_cNaviCube);
	}
	m_bInitNaviCube = pcInThat->m_bInitNaviCube;
}

// 1. BaseView를 초기화 하는 부분
bool H3DF::ViewPrivate::Init(H3DF::Model * pcInModel, const char * pchInDriverType, const char * pchInInstanceName, H3DF::WindowHandle nInWindowHandle)
{
	// HBaseView 생성
	m_pcBaseView = new H3DF::BaseView((HBaseModel*)pcInModel,
		nullptr,											// Alias
		pchInDriverType,									// Driver Type
		pchInInstanceName,									// Instance name
		reinterpret_cast<void *>(nInWindowHandle));			// Window handle

	if(nullptr == m_pcBaseView) {
		return false;
	}

	m_pcBaseView->Init();

	m_pcWindow = new WindowKey(m_pcBaseView);

	m_cNaviCube.SetView(m_pcBaseView, m_pcWindow);
	m_pcBaseView->SetNavigationCube(&m_cNaviCube);

	m_pcBaseView->GetModel()->GetEventManager()->RegisterHandler((HAnimationListener *)GetBaseView(), HAnimationListener::GetType(), HLISTENER_PRIORITY_NORMAL);

	H3DF::SelectionSet * pcSelection = new H3DF::SelectionSet(m_pcBaseView);
	pcSelection->SetAllowSubentityDeselection(true);

	m_pcBaseView->SetSelection(pcSelection);

	HMarkupManager * pcMarkupManager = new HMarkupManager(m_pcBaseView);
	m_pcBaseView->SetMarkupManager(pcMarkupManager);

	HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
		HC_Set_Selectability("everything = off");
	} HC_Close_Segment();


	// set up some scene defaults
	HC_Open_Segment_By_Key(m_pcBaseView->GetSceneKey()); {
		// #Selection: Line이 더 잘보이게 하고 선택이 잘되도록 하기 위해서 Face를 뒤로 보냄
		HC_Set_Rendering_Options("face displacement = 16"); // 양수값이 Camera에서 멀어지는 방향임.
		HC_Set_Rendering_Options("no color interpolation, color index interpolation");
		HC_Set_Visibility("lights = (faces = on, edges = off), markers = off, faces=on, edges=off, lines=on, text = on");
	} HC_Close_Segment();

	// windowspace (overlay) defaults
	HC_Open_Segment_By_Key(m_pcBaseView->GetWindowspaceKey()); {
		HC_Set_Color_By_Index("geometry", 3);
		HC_Set_Color_By_Index("window contrast", 1);
		HC_Set_Color_By_Index("windows", 1);
		HC_Set_Visibility("markers=on");
		HC_Set_Marker_Symbol("+");
		HC_Set_Selectability("off");
	} HC_Close_Segment();

	char chGpuToUse[256];
	strcpy(chGpuToUse, (char const *)H_UTF8(TheKenel.General.Display.Gpu).encodedText());
	if (strcmp(chGpuToUse, "Default") != 0) {
		HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
			HC_Set_Driver_Options(H_FORMAT_TEXT("gpu preference = specific = %s", chGpuToUse));
		} HC_Close_Segment();
	}

	// do all the setup with no updates
	m_pcBaseView->SetSuppressUpdate(true);

	m_pcBaseView->SetAxisManipulateOperator(new HOpCameraManipulate(GetBaseView(), 0, 1, new HOpCameraOrbit(GetBaseView()), new HOpCameraPan(GetBaseView()), new HOpCameraZoom(GetBaseView()), 0, false));

	HOpMoveHandle * handleoperator = new HOpMoveHandle(GetBaseView(), GetBaseView(), false);
	m_pcBaseView->SetHandleOperator(handleoperator);

	m_pcBaseView->GetEventManager()->RegisterHandler((HObjectManipulationListener *)handleoperator, HObjectManipulationListener::GetType(), HLISTENER_PRIORITY_NORMAL);
	m_pcBaseView->GetEventManager()->RegisterHandler((HJoyStickListener *)GetBaseView(), HJoyStickListener::GetType(), HLISTENER_PRIORITY_NORMAL);

	m_pcBaseView->SetKeyStateCallback(GetKeyState);
	m_pcBaseView->GetModel()->GetBhvBehaviorManager()->SetUpdateCamera(TheKenel.Interaction.Animation.UpdateCamera);

	long nDebugFlags = DEBUG_NO_WINDOWS_HOOK | DEBUG_STARTUP_CLEAR_BLACK;

	// use soft ogl if set
	if (true == TheKenel.General.Display.DriverForceSoftware) {
		nDebugFlags |= DEBUG_FORCE_SOFTWARE;
	}

	char chDriverOpts[MVO_BUFFER_SIZE], chRenderingOpts[MVO_BUFFER_SIZE] = { 0 };

	sprintf(chDriverOpts, "debug = %u", nDebugFlags);

	// set anti-aliasing if set
	if (true == TheKenel.Appearance.AntiAliasing.Use) {
		sprintf(chDriverOpts, "%s, anti-alias=%d ", chDriverOpts, TheKenel.Appearance.AntiAliasing.Level);
	}

	if (true == TheKenel.General.Display.DriverDisplayStats) {
		sprintf(chDriverOpts, "%s, display stats, display time stats, display memory stats", chDriverOpts);
	}

	if (true == TheKenel.General.Display.StereoMode) {
		sprintf(chDriverOpts, "%s, stereo", chDriverOpts);
	}

	sprintf(chDriverOpts, "%s, quick moves preference = %s", chDriverOpts, H_ASCII_TEXT(TheKenel.Selection.Highlight.QuickMovesType));

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
		HCLOCALE(sprintf(chDriverOpts, "bloom = (%s, strength=%f, blur=%d, shape=%s)",
			(TheKenel.Lighting.Bloom.Use ? "on" : "off"),
			TheKenel.Lighting.Bloom.Strength,
			TheKenel.Lighting.Bloom.Blur,
			(TheKenel.Lighting.Bloom.Shape == RadialBloom ? "radial" : "star")));
		HC_Set_Driver_Options(chDriverOpts);
		// antialiasing needs rendering option in addition to driver option
		if (true == TheKenel.Appearance.AntiAliasing.Use) {
			// Rendering Option에서는 Screen On만 설정한다.
			HC_Set_Rendering_Options("anti-alias = (screen = on)");
		}
		HC_Set_Driver_Options("special events, update interrupts");
		HC_Control_Update(".", "redraw everything");
	} HC_Close_Segment();

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

	if (true == TheKenel.Performance.FramerateOptimization.UseFramerate)
	{
		//if (!pDoc->IsFileReadDeferedForView() || CurrentFramerateMode == FramerateFixed)
		if (FramerateFixed == TheKenel.Performance.FramerateOptimization.CurrentFramerateMode)
		{
			m_pcBaseView->SetFramerateMode((FramerateMode)TheKenel.Performance.FramerateOptimization.CurrentFramerateMode,
				TheKenel.Performance.FramerateOptimization.FramerateTime, TheKenel.Performance.FramerateOptimization.MaxThreshold,
				UINT2bool(TheKenel.Performance.FramerateOptimization.UseLods), TheKenel.Performance.FramerateOptimization.DetailSteps,
				TheKenel.Performance.FramerateOptimization.HardCutoff);
		}
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
	// 2. We will have it only on the Canvas key. If required, model could have it's own
	// Rajesh B (11-Apr-2003)
	m_pcBaseView->SetPolygonHandednessMode(HandednessLeft);

	HPixelRGBA cHighlightSelectColor;
	cHighlightSelectColor.Set(255, 0, 0);

	m_pcBaseView->GetHighlightSelection()->SetSelectionFaceColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionEdgeColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionMarkerColor(cHighlightSelectColor);

	m_pcBaseView->GetSelection()->SetSelectionEdgeWeight(5.0);
	m_pcBaseView->GetHighlightSelection()->SetSelectionEdgeWeight(5.0);

	// #Selection: Highlighting Line, Edge 두께 설정
	HC_KEY nHighlightSelectionKey = m_pcBaseView->GetHighlightSelection()->GetSelectionSegment();
	HC_Open_Segment_By_Key(nHighlightSelectionKey); {
		HC_Set_Line_Weight(3.0);
		HC_Set_Edge_Weight(3.0);
	} HC_Close_Segment();

	HC_KEY nSelectionKey = m_pcBaseView->GetSelection()->GetSelectionSegment();
	HC_Open_Segment_By_Key(nSelectionKey); {
		HC_Set_Line_Weight(3.0);
		HC_Set_Edge_Weight(3.0);
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

	SetShadowColor(ThePreset.ShadowColor);

	m_pcBaseView->GetMarkupManager()->SetMarkupWeight(ThePreset.MarkupWeight / 100.0f);
	m_pcBaseView->SetShadowResolution(ThePreset.ShadowRes);
	m_pcBaseView->SetShadowBlurring(ThePreset.ShadowBlur);

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

	// 	SetDefaultOperator();
	// 	SetSuppressUpdate(false);
	// 
	// 	return;

		//we need to adjust the axis window outside the mvo class as the calculation of the window
		//extents is mfc specific

		// initialize the QueryDialog and AdvancedQueryDialog
		// Remark

	/*
		m_query_dialog = new CQueryDialog();
		m_query_dialog->Create(IDD_QUERYDIALOG);
		m_query_dialog->SetText("No entities currently under cursor.");
		m_advanced_query_dialog = new CQueryDialog();
		m_advanced_query_dialog->Create(IDD_QUERY_ADVANCED_DLG);
		m_advanced_query_dialog->SetText("No entities currently under cursor.");
	*/

	SetShowCollisions(ThePreset.ShowCollisions);

	/* // Remark
		HStreamFileToolkit * tk = GetModel()->GetStreamFileTK();
		if (tk)
		{
			// NOTE: I am setting these opcode handlers here, even if it is not a stream file. Who knows if we are in a
			// collaborative session and someone loads an hsf file - Rajesh B
			// install our custom TK_Initial_View opcode handler so that we set the initial Canvas appropriately
			tk->SetPrewalkHandler(new PartviewerHSFExtras(this));
			tk->SetOpcodeHandler(TKE_View, new PartviewerHSFExtras(this));

			if (!HDB::GetMaterialLibrary())
				HDB::SetMaterialLibrary(new HMaterialLibrary);
			tk->SetOpcodeHandler(TKE_Material, new HTK_PartviewerMaterialHandler(this, HDB::GetMaterialLibrary()));

			// set defaults on stream toolkit
			int sflags = 0;

			if (!ThePreset::bCompressVertices)
				sflags |= TK_Full_Resolution_Vertices;
			else
				tk->SetNumVertexBits(ThePreset::NumVertexBits);

			if (!ThePreset::bCompressNormals)
				sflags |= TK_Full_Resolution_Normals;
			else
				tk->SetNumNormalBits(ThePreset::NumNormalBits);

			if (!ThePreset::bCompressParameters)
				sflags |= TK_Full_Resolution_Parameters;
			else
				tk->SetNumParameterBits(ThePreset::NumParameterBits);


			if (ThePreset::bExportDictionary == true)
				sflags |= TK_Generate_Dictionary;


			if (ThePreset::bEnableInstancing == false)
				sflags |= TK_Disable_Instancing;

			if (ThePreset::bCompressConnectivity == true)
				sflags |= TK_Connectivity_Compression;

			if (ThePreset::bSaveLogFile == true)
				tk->SetLogging(true);
			else
				tk->SetLogging(false);

			// This is a paradox here, the flags passed as argument to Write_Stream function make
			// no sense if we use our own toolkit. These flags need to be set on the toolkit itself
			tk->SetWriteFlags(sflags);

			// Anything <= 0 for export version means use the default.
			if (ThePreset::HsfExportVersion > 0)
				tk->SetTargetVersion(ThePreset::HsfExportVersion);
		}*/


	/*
		if (pDoc->IsFileReadDeferedForView())
			LoadFile(pDoc->filename, hmodel->GetStreamFileTK());
		else
		{
			SetLineAntialiasing(ThePreset.LineAntialiasing);
			SetTextAntialiasing(ThePreset.TextAntialiasing);

			EmitSegment(GetModel()->GetModelKey(), true);

			SetupViews();

			if (ThePreset.UseFramerate && ThePreset.CurrentFramerateMode == FramerateTarget)
				EnableFrameRate();

			ViewReady();
		}
	*/

	SetSceneFont(ThePreset.FontName, ThePreset.FontSize, ThePreset.FontUnits);

	// subscribe to selection events
// 	m_nCookieSelected = SetSignalNotify(HSignalSelected, CSolidHoopsView::signal_selected, this);
// 	m_nCookieDeSelectedAll = SetSignalNotify(HSignalDeSelectedAll, CSolidHoopsView::signal_deselected_all, this);

	// Remark
// 	CSolidHoopsFrame * frame = (CSolidHoopsFrame *)AfxGetMainWnd();
// 	frame->GetBhvToolbar()->m_wndBhvSlider.SetPos(0);

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

	HC_Open_Segment_By_Key(m_pcBaseView->GetShadowMapSegmentKey()); {
		if (ThePreset.ShadowMap) {
			sprintf(chRenderingOpts, "shadow map=(on, resolution=%d, samples=%d, %s jitter, %s)",
				ThePreset.SMResolution, ThePreset.SMSamples,
				(ThePreset.Jitter ? "" : "no"),
				(ThePreset.ViewDependentShadowMap ? "Canvas dependent" : "Canvas independent"));
		}
		else {
			sprintf(chRenderingOpts, "no shadow map");
		}
		HC_Set_Rendering_Options(chRenderingOpts);
	} HC_Close_Segment();

	HC_Open_Segment_By_Key(m_pcBaseView->GetSceneKey()); {
		HC_Set_Variable_Edge_Weight(H_ASCII_TEXT(ThePreset.LineWeight));
		HC_Set_Variable_Line_Weight(H_ASCII_TEXT(ThePreset.LineWeight));

		//apply stereo mode
		if (ThePreset.StereoMode) {
			HCLOCALE(sprintf(chRenderingOpts, "stereo, stereo separation = %f", ThePreset.StereoSeparation / 10000.f));
			HC_Set_Rendering_Options(chRenderingOpts);
		}

		HCLOCALE(sprintf(chRenderingOpts, "simple shadow = (opacity = %f)", ThePreset.ShadowOpacity));
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
			HC_Set_Color_Map(H_ASCII_TEXT(ThePreset.GoochColorMap));
		}HC_Close_Segment();

		HCLOCALE(sprintf(chRenderingOpts, "gooch options = (diffuse weight = %f, color range=(0.0, %f), color map segment = `./overwrite/lights/gooch_color_map_segment`)",
			ThePreset.GoochWeight, ThePreset.GoochColorHigh));
		HC_Set_Rendering_Options(chRenderingOpts);

		//set camera near limit
		HC_Set_Camera_Near_Limit(ThePreset.NearCameraLimit / 100000.f);

		//Apply curve geometry options
		char curve_opt[4096];
		HCLOCALE(sprintf(curve_opt, "general curve = (budget = %d, continued budget = %d, maximum deviation = %f, maximum angle = %f, maximum length = %f, %s Canvas independent)",
			ThePreset.Budget, ThePreset.ContinuedBudget, ThePreset.MaxDeviation / 10000.f,
			ThePreset.MaxAngle / 10000.f, ThePreset.MaxLength / 10000.f, ThePreset.ViewIndependent ? "" : "no"));

		HC_Set_Rendering_Options(curve_opt);

		m_pcBaseView->SetReflectionPlane(ThePreset.ReflectionPlane, ThePreset.ReflectionOpacity,
			ThePreset.ReflectionFading, ThePreset.ReflectionUseAttenuation,
			ThePreset.ReflectionHither, ThePreset.ReflectionYon,
			ThePreset.ReflectionUseBlur, ThePreset.ReflectionBlur);

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
			strcpy(cGreekingSettings, H_ASCII_TEXT(csGreekingSettings));
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

	SetDefaultOperator();

	// Object Snap용 Glyph 생성
	//Operator::ObjectSnap::CreateGlyph();

	SetSelectOption();

	// do all the setup with no updates
	m_pcBaseView->SetSuppressUpdate(false);

	return true;
}

void H3DF::ViewPrivate::InitNavigationCube(int nWidth, int nHeight)
{
	//m_cNaviCube.SetView(m_pcBaseView, m_pcWindow);
	m_cNaviCube.Create(nWidth, nHeight, m_pcBaseView->GetModelKey());
	m_cNaviCube.Transform();

	//m_pcBaseView->SetNavigationCube(&m_cNaviCube);

	m_bInitNaviCube = true;
}

bool H3DF::ViewPrivate::GetKeyState(unsigned int key, int & flags)
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

void H3DF::ViewPrivate::SetMarkupColor(COLORREF new_color, bool emit_message)
{
	UNREFERENCED(emit_message);

	HPoint new_mkp_color;
	new_mkp_color.Set(ColorValue(new_color));

	HC_Open_Segment_By_Key(m_pcBaseView->GetMarkupManager()->GetMarkupKey()); {
		HC_Set_Color_By_Value("everything", "RGB", new_mkp_color.x, new_mkp_color.y, new_mkp_color.z);
	}HC_Close_Segment();

	m_pcBaseView->GetMarkupManager()->SetMarkupColor(new_mkp_color);
}

void H3DF::ViewPrivate::SetShadowColor(COLORREF new_color)
{
	HPoint new_shd_color;
	new_shd_color.Set(ColorValue(new_color));
	m_pcBaseView->SetShadowColor(new_shd_color);
}

void H3DF::ViewPrivate::event_checker(HIC_Rendition const * nr)
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

void H3DF::ViewPrivate::SetViewAxis()
{
	char text[4096];
	HVector front, top;
	CString strViewAxis = " 1  0  0  0  1  0  0  0  1";
	strcpy(text, H_ASCII_TEXT(strViewAxis));
	sscanf(text, "%f %f %f %f %f %f", &front.x, &front.y, &front.z,
		&top.x, &top.y, &top.z);

	m_pcBaseView->SetViewAxis(&front, &top);
}

// 투명도 적용 방법 설정
void H3DF::ViewPrivate::SetTransparency()
{
	char text[4096];
	char style[4096];
	char sorting[4096];
	char layers[4096];
	bool fast_z_sort = false;

	strcpy(style, H_ASCII_TEXT(TheKenel.General.Transparency.Style));
	strcpy(sorting, H_ASCII_TEXT(TheKenel.General.Transparency.Sorting));
	strcpy(layers, H_ASCII_TEXT(TheKenel.General.Transparency.DepthPeelingLayers));

	if (strstr(sorting, "z-sort")) {
		if (strstr(sorting, "fast")) {
			fast_z_sort = true;
		}
		sprintf(sorting, "z-sort only");
	}

	sprintf(text, "style = %s, hsr algorithm = %s, depth peeling options = (layers= %s, algorithm=%s), depth writing = %s",
		style, sorting, layers, TheKenel.General.Transparency.PixelOIT ? "pixel" : "buffer",
		TheKenel.General.Transparency.DepthWriting == true ? "on" : "off");

	m_pcBaseView->SetTransparency(text, fast_z_sort);
}

// Select option 처리
void H3DF::ViewPrivate::SetSelectOption()
{
	MaterialMappingKit cMaterial;
	cMaterial.SetFaceColor(RGBColor(1.0f, 0.5f, 0.0f));
	cMaterial.SetEdgeColor(RGBColor(1.0f, 0.5f, 0.0f));
	cMaterial.SetLineColor(RGBColor(1.0f, 0.5f, 0.0f));

	// #Selection: Selection Option 설정 
	m_pcWindow->GetSelectionOptionsControl().SetLevel(Selection::Level::Entity);
	m_pcWindow->GetSelectionOptionsControl().SetRelatedLimit(10);
	m_pcWindow->GetSelectionOptionsControl().SetProximity(0.2f);
	//m_pcWindow->GetSelectionOptionsControl().SetBias(Selection::Bias::Lines);
	//m_pcWindow->GetSelectionOptionsControl().SetSorting(Selection::Sorting::Proximity); // Sorting 해도 Z방향 Sort가 정확하게 되지는 않됨.

	m_pcWindow->GetHighlightControl().SetMaterialMapping(cMaterial);
	m_pcWindow->GetHighlightControl().GetLineAttributeControl().SetWeight(5.0);

	return;


	HPixelRGBA cHighlightSelectColor;
	cHighlightSelectColor.Set(255, 0, 0);
	m_pcBaseView->GetHighlightSelection()->SetSelectionFaceColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionEdgeColor(cHighlightSelectColor);
	m_pcBaseView->GetHighlightSelection()->SetSelectionMarkerColor(cHighlightSelectColor);

	m_pcBaseView->GetHighlightSelection()->SetGrayScale(ThePreset.GrayScaleSelection);
	m_pcBaseView->GetHighlightSelection()->SetUseDefinedHighlight(ThePreset.UseDefinedHighlighting);
	m_pcBaseView->GetHighlightSelection()->SetInvisible(ThePreset.InvisibleSelection);
	m_pcBaseView->GetHighlightSelection()->SetAllowDisplacement(ThePreset.DisplaceSelection);
	m_pcBaseView->SetDynamicHighlighting(TheKenel.Selection.Behavior.DynamicHighlighting);
	m_pcBaseView->GetHighlightSelection()->UpdateHighlightStyle();

	char chDriverOpts[MVO_BUFFER_SIZE];
	sprintf(chDriverOpts, "quick moves preference = %s", H_ASCII_TEXT(TheKenel.Selection.Highlight.QuickMovesType));
	HC_Open_Segment_By_Key(m_pcBaseView->GetViewKey()); {
		HC_Set_Driver_Options(chDriverOpts);
	} HC_Close_Segment();

	// set the selection color
	HSelectionSet * sel_set = m_pcBaseView->GetSelection();
	assert(sel_set);
	HPixelRGBA sel_col;
	int sel_alpha = (int)(ThePreset.SelectionColorTransparency * 2.56f);		// settings is a %, scale it to 256
	sel_col.Set(ColorRGBA(ThePreset.PolygonSelectionColor, sel_alpha));
	sel_set->SetSelectionFaceColor(sel_col);

	sel_col.Set(ColorRGBA(ThePreset.LineSelectionColor, sel_alpha));
	sel_set->SetSelectionEdgeColor(sel_col);

	sel_col.Set(ColorRGBA(ThePreset.MarkerSelectionColor, sel_alpha));
	sel_set->SetSelectionMarkerColor(sel_col);

	m_pcBaseView->GetSelection()->SetGrayScale(ThePreset.GrayScaleSelection);
	m_pcBaseView->GetSelection()->SetUseDefinedHighlight(ThePreset.UseDefinedHighlighting);
	m_pcBaseView->GetSelection()->SetAllowDisplacement(ThePreset.DisplaceSelection);
	m_pcBaseView->GetSelection()->SetInvisible(ThePreset.InvisibleSelection);
	m_pcBaseView->GetSelection()->SetHighlightMode((HSelectionHighlightMode)ThePreset.HighlightMode);

	m_pcBaseView->GetHighlightSelection()->SetHighlightMode((HSelectionHighlightMode)ThePreset.HighlightMode);
	m_pcBaseView->GetSelection()->SetHighlightTransparency(ThePreset.TransparencyLevel);

	if (ThePreset.RefSelType == "Spriting")
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelSpriting);
	else if (ThePreset.RefSelType == "Off")
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelOff);
	else
		m_pcBaseView->GetSelection()->SetReferenceSelectionType(RefSelDefault);

	m_pcBaseView->GetSelection()->UpdateHighlightStyle();
}


void H3DF::ViewPrivate::SetWindowBackGroundColor(COLORREF nNewTopColor, COLORREF nNewBottomColor, bool bEmitMessage)
{
	HPoint nWindowTopColor;
	nWindowTopColor.Set(ColorValue(nNewTopColor));

	HPoint nWindowBottomColor;
	nWindowBottomColor.Set(ColorValue(nNewBottomColor));

	m_pcBaseView->SetWindowColor(nWindowTopColor, nWindowBottomColor, bEmitMessage);
}

void H3DF::ViewPrivate::SetSceneFont(CString csFontName, CString csFontSize, CString csFontUnits)
{
	HC_Open_Segment_By_Key(GetBaseView()->GetSceneKey()); {
		// first let's query the user's font size settings
		float size = (float)atof(H_ASCII_TEXT(csFontSize));
		if (size < 0) {
			size *= -1;
		}

		char cfname[MVO_BUFFER_SIZE];
		sprintf(cfname, "name = \"%s\"", (const char *)H_ASCII_TEXT(csFontName));
		HC_Set_Text_Font(cfname);

		// set the font size via MVO - to propogate it to the hnet clients
		// hnet removed: do we still need to do this?
		char cfsize[MVO_BUFFER_SIZE];
		HCLOCALE(sprintf(cfsize, "%f %s", size, (const char *)H_ASCII_TEXT(csFontUnits)));

		GetBaseView()->SetFontSize(cfsize, true);

	} HC_Close_Segment();
}

//== Operator 관련 함수 ==============================================================================

void H3DF::ViewPrivate::SetDefaultOperator()
{
	//m_pcCameraManipulate = new HOpCameraManipulate(this, 0, 1, new OpCameraOrbitSelect(this), new OpCameraPan(this));
// 		, new HSOpCameraPan(m_pHView),
// 		new HSOpCameraZoom(m_pHView), 0, false))

	m_pcCameraOrbitSelect = new Operator::CameraSelect(m_pcWindow, m_cNaviCube);
	m_pcSelectArea = new Operator::SelectArea(GetBaseView());

	GetBaseView()->SetOperator(m_pcCameraOrbitSelect);

	//LocalSetOperator(m_pcCameraManipulate);
}

void H3DF::ViewPrivate::LocalSetOperator(HBaseOperator * pcNewOperator)
{
	HBaseOperator * pcOperator = GetBaseView()->GetOperator();
	GetBaseView()->SetOperator(pcNewOperator);

	if (nullptr != pcOperator) {
		delete pcOperator;
	}
}

//== Mouse Function ================================================================================

bool H3DF::ViewPrivate::LButtonDown(int nFlags, int x, int y)
{
	// GetBaseView()->SetDynamicHighlighting(false);

	// Shift & L Button 이벤트는 Area Select
	if (MK_SHIFT & nFlags) {
		GetBaseView()->SetOperator(m_pcSelectArea);
	}
	else {
		GetBaseView()->SetOperator(m_pcCameraOrbitSelect);
	}

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_LButtonDown, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnLButtonDown(cEvent));
	return true;
}

bool H3DF::ViewPrivate::LButtonUp(int nFlags, int x, int y)
{
	// GetBaseView()->SetDynamicHighlighting(true);

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_LButtonUp, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnLButtonUp(cEvent));

	HBaseOperator * op = GetBaseView()->GetCurrentOperator();

	if (op) {
		if (op->Capture()) {

		}
	}

	GetBaseView()->SetOperator(m_pcCameraOrbitSelect);

	return true;
}


bool H3DF::ViewPrivate::RButtonDown(int nFlags, int x, int y)
{
	//GetBaseView()->SetDynamicHighlighting(false);

	GetBaseView()->SetOperator(m_pcCameraOrbitSelect);

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_RButtonDown, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnRButtonDown(cEvent));
	return true;
}

bool H3DF::ViewPrivate::RButtonUp(int nFlags, int x, int y)
{
	//GetBaseView()->SetDynamicHighlighting(true);

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_RButtonUp, x, y, MouseMapFlags(nFlags));
	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnRButtonUp(cEvent));

	return true;
}

bool H3DF::ViewPrivate::MouseMove(int nFlags, int x, int y)
{
	// Control을 누른경우 Face 단위로 선택이 됨.
/*
	if (nFlags & MK_CONTROL) {
		// select on arbitrary subentities(face, edge, or vertex)
		GetHighlightSelection()->SetSelectionLevel(HSelectLevel::HSelectEntity);
	}
	else {
		GetHighlightSelection()->SetSelectionLevel(HSelectLevel::HSelectSegment);
	}
*/
	BaseView * pcView = GetBaseView();

	HEventInfo cEvent(GetBaseView());
	cEvent.SetPoint(HE_MouseMove, x, y, MouseMapFlags(nFlags));

	/*
		if (MK_LBUTTON & nFlags || MK_RBUTTON & nFlags) {
			GetBaseView()->GetOperator()->OnMouseMove(cEvent);
		}
		else {
			m_pcWindow->OnMouseMove(cEvent);
		}
	*/

	HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnMouseMove(cEvent));

	return true;
}

// Mouse Wheel 대응
bool H3DF::ViewPrivate::MouseWheel(int nFlags, int zDelta, int x, int y, int nLeft, int nTop)
{
	HEventInfo	cEvent(GetBaseView());
	cEvent.SetPoint(HE_MouseWheel, x - nLeft, y - nTop, MouseMapFlags(nFlags));
	cEvent.SetMouseWheelDelta(zDelta);
	m_pcCameraOrbitSelect->OnMouseWheel(cEvent);

	//HLISTENER_EVENT(HMouseListener, GetBaseView()->GetEventManager(), OnMouseWheel(cEvent));

	return true;
}

//== Keyboard 관련 함수 ==============================================================================
bool H3DF::ViewPrivate::Char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		default:
			break;
	}

	return true;
}

void H3DF::ViewPrivate::ViewReady()
{
	HBaseModel * hmodel = GetBaseView()->GetModel();

	GetBaseView()->SetSuppressUpdate(true);

	GetBaseView()->SetSplatRendering(BOOL2bool(ThePreset.SplatRendering));

	GetBaseView()->SetFastMarkerDrawing(ThePreset.FastMarkers);

	HC_Open_Segment_By_Key(GetBaseView()->GetShadowMapSegmentKey()); {
		char opt[MVO_BUFFER_SIZE];

		sprintf(opt, "shadow map=(%s, resolution=%d, samples=%d, %s jitter)",
			ThePreset.ShadowMap ? "on" : "off",
			ThePreset.SMResolution, ThePreset.SMSamples, ThePreset.Jitter ? "" : "no");

		HC_Set_Rendering_Options(opt);
	} HC_Close_Segment();


	HC_Open_Segment_By_Key(GetBaseView()->GetSceneKey()); {
		if (ThePreset.ShadowMap) {
			HC_Set_Visibility("shadows = (emitting, casting, receiving)");
		}

		char opt[MVO_BUFFER_SIZE] = "";
		char refl_opt[MVO_BUFFER_SIZE];

		HCLOCALE(sprintf(opt, "simple reflection=(%s, opacity=%f, fading= %s, ",
			ThePreset.ReflectionPlane ? "on" : "off",
			ThePreset.ReflectionOpacity, ThePreset.ReflectionFading ? "on" : "off"));

		if (ThePreset.ReflectionUseAttenuation) {
			HCLOCALE(sprintf(refl_opt, "attenuation = (hither=%f, yon=%f), ",
				ThePreset.ReflectionHither, ThePreset.ReflectionYon));
		}
		else
			sprintf(refl_opt, "no attenuation, ");
		strcat(opt, refl_opt);

		if (ThePreset.ReflectionUseBlur)
			sprintf(refl_opt, "blur=%d)", ThePreset.ReflectionBlur);
		else
			sprintf(refl_opt, "no blur)");
		strcat(opt, refl_opt);

		HC_Set_Rendering_Options(opt);
	} HC_Close_Segment();

	GetBaseView()->SetShadowLightDirection(ThePreset.UseLightVector, (HPoint *)&ThePreset.LightVector);
	GetBaseView()->SetShadowIgnoresTransparency(ThePreset.IgnoreTransparency);
	GetBaseView()->SetShadowMode((HShadowMode)ThePreset.ShadowMode);
	GetBaseView()->SetOcclusionCullingMode(ThePreset.OcclusionCulling);
	GetBaseView()->SetLineAntialiasing(TheKenel.Appearance.AntiAliasing.Line);
	GetBaseView()->SetTextAntialiasing(TheKenel.Appearance.AntiAliasing.Text);

	SetTransparency();

	//Turn on static model and display lists last, and in that order
	hmodel->SetStaticModel(TheKenel.Performance.Optimization.StaticModel);

	hmodel->SetLMVModel(TheKenel.Performance.Optimization.LMVModel);

	if (ThePreset.RestoreAnnotations) {
		GetBaseView()->SetAnnotationResize(true);
	}

	if (DisplayListOff == ThePreset.DisplayList) {
		GetBaseView()->SetDisplayListMode(false);
	}
	else {
		GetBaseView()->SetDisplayListType((DisplayListType)ThePreset.DisplayList);
		GetBaseView()->SetDisplayListMode(true);
	}

	// Check whether this file contains layout, if yes, load them (applicable to dwg files, and hsfs saved from
	// dwg files)
/* // Remark
	bool is_layouts = false;
	is_layouts = HDWGLayoutLoadUtility::CheckForLayoutSegmentInThisModel(GetModelKey());
	if (is_layouts)
	{
		m_pLayoutLoadUtility = new HDWGLayoutLoadUtility(m_pHView);
		m_pLayoutLoadUtility->LoadLayoutList();
	}
*/

	GetBaseView()->SetHandednessFromModel();

	GetBaseView()->SetSuppressUpdate(false);
}

#include "../3DF/Operator.KeyboardTest.h"
//:TEMP
Operator::KeyboardTest * g_pOperator = nullptr;

bool H3DF::ViewPrivate::KeyboardInput(Json::Object & cInObject)
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

DWORD H3DF::ViewPrivate::MouseMapFlags(DWORD nState)
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

//== Select 관련 함수 ================================================================================

// 선택된 Entity 선택 해제
void H3DF::ViewPrivate::DeSelectAll()
{
	if (0 < GetBaseView()->GetSelection()->GetSize()) {
		GetBaseView()->GetSelection()->DeSelectAll();
		GetBaseView()->ForceUpdate();
	}
}

void H3DF::ViewPrivate::SetSubentitySelectLevel()
{
	HSelectionSet * pcSelection = GetBaseView()->GetSelection();

	if (HSelectLevel::HSelectSubentity != pcSelection->GetSelectionLevel()) {
		pcSelection->DeSelectAll();
		GetBaseView()->Update();
	}
	pcSelection->SetSelectionLevel(HSelectLevel::HSelectSubentity);
	GetBaseView()->SetViewSelectionLevel(HSelectionLevelEntity);
	GetBaseView()->Update();
}
