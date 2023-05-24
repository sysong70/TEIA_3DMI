#include "StdAfx.h"

#include "3DF.BaseView.h"

#include "3DF.NavigationCube.h"

#include <HBaseModel.h>

USING_3DF_NAMESPACE

TDF::BaseView::BaseView(HBaseModel * model, const char * alias, const char * driver_type, const char * instance_name,
	void * window_handle, void * colormap, void * clip_override, void * window_handle_2, const char * driver_path)
	: HBaseView(model, alias, driver_type, instance_name, window_handle, colormap, clip_override, window_handle_2, driver_path)
{
}

void TDF::BaseView::UpdateInternal(bool antialias, bool force_update)
{
	if (nullptr != m_pcNaviCube) {
		m_pcNaviCube->Transform();
	}

	HBaseView::UpdateInternal(antialias, force_update);
}

void TDF::BaseView::SetViewMode(TDF::ViewMode eViewMode, bool bFitWorld)
{
	TDF::ViewMode eOldViewMode = m_eViewMode;
	m_eViewMode = eViewMode;

	if (TDF::ViewMode::Unknown == eViewMode) {
		return;
	}

	HPoint target, camera, view;
	float length;

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

		length = (float)HC_Compute_Vector_Length(&view);

		float newLen = length * 0.5774f;

		HPoint rightaxis;
		HC_Compute_Cross_Product(&m_FrontAxis, &m_TopAxis, &rightaxis);

		if (GetHandedness() == HandednessRight) {
			rightaxis.Set(-rightaxis.x, -rightaxis.y, -rightaxis.z);
		}

		float px = target.x + newLen * m_FrontAxis.x - newLen * rightaxis.x + newLen * m_TopAxis.x;
		float py = target.y + newLen * m_FrontAxis.y - newLen * rightaxis.y + newLen * m_TopAxis.y;
		float pz = target.z + newLen * m_FrontAxis.z - newLen * rightaxis.z + newLen * m_TopAxis.z;

		float fCos45 = cos(M_PI / 4);
		float fLenCos = length * fCos45;

		HVector cLenFrontAxis = m_FrontAxis * length;
		HVector cTopAxis = m_TopAxis * length;
		HVector cRightAxis = rightaxis * length;

		switch (m_eViewMode) {

			case TDF::ViewMode::right: {
				HC_Set_Camera_Position(target.x + cLenFrontAxis.x, target.y + cLenFrontAxis.y, target.z + cLenFrontAxis.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 0, 1);
				}
				else {
					HC_Set_Camera_Up_Vector(0, -cUpVector.z, cUpVector.y);
				}
			} break;

			case TDF::ViewMode::left: {
				HC_Set_Camera_Position(target.x - cLenFrontAxis.x, target.y + cLenFrontAxis.y, target.z + cLenFrontAxis.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 0, 1);
				}
				else {
					HC_Set_Camera_Up_Vector(0, cUpVector.z, -cUpVector.y);
				}
			} break;

			case TDF::ViewMode::front: {
				HC_Set_Camera_Position(target.x + cTopAxis.x, target.y - cTopAxis.y, target.z + cTopAxis.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 0, 1);
				}
				else {
					HC_Set_Camera_Up_Vector(-cUpVector.z, 0, cUpVector.x);
				}
			} break;

			case TDF::ViewMode::back: {
				HC_Set_Camera_Position(target.x + cTopAxis.x, target.y + cTopAxis.y, target.z + cTopAxis.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 0, 1);
				}
				else {
					HC_Set_Camera_Up_Vector(cUpVector.z, 0, -cUpVector.x);
				}
			} break;

			case TDF::ViewMode::top: {
				HC_Set_Camera_Position(target.x + cRightAxis.x, target.y + cRightAxis.y, target.z - cRightAxis.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 1, 0);
				}
				else {
					HC_Set_Camera_Up_Vector(-cUpVector.y, cUpVector.x, 0);
				}
			} break;

			case TDF::ViewMode::bottom: {
				HC_Set_Camera_Position(target.x + cRightAxis.x, target.y + cRightAxis.y, target.z + cRightAxis.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 1, 0);
				}
				else {
					HC_Set_Camera_Up_Vector(cUpVector.y, -cUpVector.x, 0);
				}
			} break;

			case TDF::ViewMode::py_nz: { // Back - Bottom
				HC_Set_Camera_Position(target.x, target.y + fLenCos, target.z - fLenCos);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, fCos45, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.y - fCos45)) {
						HC_Set_Camera_Up_Vector(1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - 1)) {
						HC_Set_Camera_Up_Vector(0, -fCos45, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.y + fCos45)) {
						HC_Set_Camera_Up_Vector(-1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + 1)) {
						HC_Set_Camera_Up_Vector(0, fCos45, fCos45);
					}
				}
			} break;

			case TDF::ViewMode::py_pz: { // Top - Back
				HC_Set_Camera_Position(target.x, target.y + fLenCos, target.z + fLenCos);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, -fCos45, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.y + fCos45)) {
						HC_Set_Camera_Up_Vector(1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - 1)) {
						HC_Set_Camera_Up_Vector(0, fCos45, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.y - fCos45)) {
						HC_Set_Camera_Up_Vector(-1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + 1)) {
						HC_Set_Camera_Up_Vector(0, -fCos45, fCos45);
					}
				}
			} break;

			case TDF::ViewMode::ny_pz: { // Top - Front
				HC_Set_Camera_Position(target.x, target.y - fLenCos, target.z + fLenCos);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, fCos45, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.y - fCos45)) {
						HC_Set_Camera_Up_Vector(-1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + 1)) {
						HC_Set_Camera_Up_Vector(0, -fCos45, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.y + fCos45)) {
						HC_Set_Camera_Up_Vector(1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - 1)) {
						HC_Set_Camera_Up_Vector(0, fCos45, fCos45);
					}
				}
			} break;

			case TDF::ViewMode::ny_nz: { // Front - Bottom
				HC_Set_Camera_Position(target.x, target.y - fLenCos, target.z - fLenCos);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, -fCos45, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.y + fCos45)) {
						HC_Set_Camera_Up_Vector(-1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + 1)) {
						HC_Set_Camera_Up_Vector(0, fCos45, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.y - fCos45)) {
						HC_Set_Camera_Up_Vector(1, 0, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - 1)) {
						HC_Set_Camera_Up_Vector(0, -fCos45, fCos45);
					}
				}
			} break;

			case TDF::ViewMode::nx_nz: { // Left - Bottom
				HC_Set_Camera_Position(target.x - fLenCos, target.y, target.z - fLenCos);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(-fCos45, 0, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.x + fCos45)) {
						HC_Set_Camera_Up_Vector(0, 1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						HC_Set_Camera_Up_Vector(fCos45, 0, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						HC_Set_Camera_Up_Vector(0, -1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y + 1)) {
						HC_Set_Camera_Up_Vector(-fCos45, 0, fCos45);
					}
				}
			} break;

			case TDF::ViewMode::nx_pz: {
				HC_Set_Camera_Position(target.x - fLenCos, target.y, target.z + fLenCos);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(fCos45, 0, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.x - fCos45)) {
						HC_Set_Camera_Up_Vector(0, 1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						HC_Set_Camera_Up_Vector(-fCos45, 0, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						HC_Set_Camera_Up_Vector(0, -1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y + 1)) {
						HC_Set_Camera_Up_Vector(fCos45, 0, fCos45);
					}
				}
			} break;

			case TDF::ViewMode::px_pz: { // Top - Right
				HC_Set_Camera_Position(target.x + fLenCos, target.y, target.z + fLenCos);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(-fCos45, 0, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.x + fCos45)) {
						HC_Set_Camera_Up_Vector(0, -1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y + 1)) {
						HC_Set_Camera_Up_Vector(fCos45, 0, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						HC_Set_Camera_Up_Vector(0, 1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						HC_Set_Camera_Up_Vector(-fCos45, 0, fCos45);
					}
				}
			} break;

			case TDF::ViewMode::px_nz: { // Right - Bottom
				HC_Set_Camera_Position(target.x + fLenCos, target.y, target.z - fLenCos);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(fCos45, 0, fCos45);
				}
				else {
					if (1e-6 > fabs(cUpVector.x - fCos45)) {
						HC_Set_Camera_Up_Vector(0, -1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y + 1)) {
						HC_Set_Camera_Up_Vector(-fCos45, 0, -fCos45);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						HC_Set_Camera_Up_Vector(0, 1, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						HC_Set_Camera_Up_Vector(fCos45, 0, fCos45);
					}
				}
			} break;

			case TDF::ViewMode::nx_py: { // Back - Left
				HC_Set_Camera_Position(target.x - fLenCos, target.y + fLenCos, target.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 0, 1);
				}
				else {
					if (1e-6 > fabs(cUpVector.z - 1)) {
						HC_Set_Camera_Up_Vector(fCos45, fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						HC_Set_Camera_Up_Vector(0, 0, -1);
					}
					else if (1e-6 > fabs(cUpVector.z + 1)) {
						HC_Set_Camera_Up_Vector(-fCos45, -fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						HC_Set_Camera_Up_Vector(0, 0, 1);
					}
				}
			} break;

			case TDF::ViewMode::px_py: { // Right - Back
				HC_Set_Camera_Position(target.x + fLenCos, target.y + fLenCos, target.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 0, 1);
				}
				else {
					if (1e-6 > fabs(cUpVector.z - 1)) {
						HC_Set_Camera_Up_Vector(fCos45, -fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						HC_Set_Camera_Up_Vector(0, 0, -1);
					}
					else if (1e-6 > fabs(cUpVector.z + 1)) {
						HC_Set_Camera_Up_Vector(-fCos45, fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						HC_Set_Camera_Up_Vector(0, 0, 1);
					}
				}
			} break;

			case TDF::ViewMode::px_ny: { // Front - Right
				HC_Set_Camera_Position(target.x + fLenCos, target.y - fLenCos, target.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 0, 1);
				}
				else {
					if (1e-6 > fabs(cUpVector.z - 1)) {
						HC_Set_Camera_Up_Vector(-fCos45, -fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						HC_Set_Camera_Up_Vector(0, 0, -1);
					}
					else if (1e-6 > fabs(cUpVector.z + 1)) {
						HC_Set_Camera_Up_Vector(fCos45, fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x - fCos45)) {
						HC_Set_Camera_Up_Vector(0, 0, 1);
					}
				}
			} break;

			case TDF::ViewMode::nx_ny: { // Left - Front
				HC_Set_Camera_Position(target.x - fLenCos, target.y - fLenCos, target.z);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0, 0, 1);
				}
				else {
					if (1e-6 > fabs(cUpVector.z - 1)) {
						HC_Set_Camera_Up_Vector(-fCos45, fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.x + fCos45)) {
						HC_Set_Camera_Up_Vector(0, 0, -1);
					}
					else if (1e-6 > fabs(cUpVector.z + 1)) {
						HC_Set_Camera_Up_Vector(fCos45, -fCos45, 0);
					}
					else if (1e-6 > fabs(cUpVector.y - 1)) {
						HC_Set_Camera_Up_Vector(0, 0, 1);
					}
				}
			} break;

			case TDF::ViewMode::nx_py_nz: {
				HC_Set_Camera_Position(-px, py, -pz);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(-0.408248f, 0.408249f, 0.816497f);
				}
				else {
					HC_Set_Camera_Up_Vector(cUpVector.z, -cUpVector.x, -cUpVector.y);
				}
			} break;

			case TDF::ViewMode::nx_py_pz: {
				HC_Set_Camera_Position(-px, py, pz);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0.408248f, -0.408249f, 0.816497f);
				}
				else {
					HC_Set_Camera_Up_Vector(-cUpVector.y, cUpVector.z, -cUpVector.x);
				}
			} break;

			case TDF::ViewMode::nx_ny_pz: {
				HC_Set_Camera_Position(-px, -py, pz);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0.408248f, 0.408249f, 0.816497f);
				}
				else {
					HC_Set_Camera_Up_Vector(-cUpVector.z, cUpVector.x, -cUpVector.y);
				}
			} break;

			case TDF::ViewMode::nx_ny_nz: {
				HC_Set_Camera_Position(-px, -py, -pz);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(-0.408248f, -0.408248f, 0.816497f);
				}
				else {
					HC_Set_Camera_Up_Vector(cUpVector.y, cUpVector.z, cUpVector.x);
				}
			} break;

			case TDF::ViewMode::px_py_pz: { // ISO
				HC_Set_Camera_Position(px, py, pz);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(-0.408248f, -0.408249f, 0.816497f);
				}
				else {
					HC_Set_Camera_Up_Vector(cUpVector.z, cUpVector.x, cUpVector.y);
				}
			} break;

			case TDF::ViewMode::px_py_nz: {
				HC_Set_Camera_Position(px, py, -pz);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0.468564f, 0.317602f, 0.824364f);
				}
				else {
					HC_Set_Camera_Up_Vector(cUpVector.y, -cUpVector.z, -cUpVector.x);
				}
				
			} break;

			case ViewMode::px_ny_nz: {
				HC_Set_Camera_Position(px, -py, -pz);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(0.468564f, -0.317602f, 0.824364f);
				}
				else {
					HC_Set_Camera_Up_Vector(-cUpVector.z, -cUpVector.x, cUpVector.y);
				}

			} break;

			case ViewMode::px_ny_pz: {
				HC_Set_Camera_Position(px, -py, pz);
				if (eViewMode != eOldViewMode) {
					HC_Set_Camera_Up_Vector(-0.408249f, 0.408248f, 0.816497f);
				}
				else {
					HC_Set_Camera_Up_Vector(-cUpVector.y, -cUpVector.z, cUpVector.x);
				}
				
			} break;

			default:
				assert(0);
				break;
		}

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

void TDF::BaseView::SetNavigationCube(NavigationCube * pcNaviCube)
{
	m_pcNaviCube = pcNaviCube;
}