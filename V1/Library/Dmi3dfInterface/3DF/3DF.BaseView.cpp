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

			case TDF::ViewMode::right: {
				cSetPosition.Set(target.x + cLenFrontAxis.x, target.y + cLenFrontAxis.y, target.z + cLenFrontAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(0, -cUpVector.z, cUpVector.y);
				}
			} break;

			case TDF::ViewMode::left: {
				cSetPosition.Set(target.x - cLenFrontAxis.x, target.y + cLenFrontAxis.y, target.z + cLenFrontAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(0, cUpVector.z, -cUpVector.y);
				}
			} break;

			case TDF::ViewMode::front: {
				cSetPosition.Set(target.x + cLenTopAxis.x, target.y - cLenTopAxis.y, target.z + cLenTopAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(-cUpVector.z, 0, cUpVector.x);
				}
			} break;

			case TDF::ViewMode::back: {
				cSetPosition.Set(target.x + cLenTopAxis.x, target.y + cLenTopAxis.y, target.z + cLenTopAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 0, 1);
				}
				else {
					cSetUpVector.Set(cUpVector.z, 0, -cUpVector.x);
				}
			} break;

			case TDF::ViewMode::top: {
				cSetPosition.Set(target.x + cLenRightAxis.x, target.y + cLenRightAxis.y, target.z - cLenRightAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 1, 0);
				}
				else {
					cSetUpVector.Set(-cUpVector.y, cUpVector.x, 0);
				}
			} break;

			case TDF::ViewMode::bottom: {
				cSetPosition.Set(target.x + cLenRightAxis.x, target.y + cLenRightAxis.y, target.z + cLenRightAxis.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0, 1, 0);
				}
				else {
					cSetUpVector.Set(cUpVector.y, -cUpVector.x, 0);
				}
			} break;

			case TDF::ViewMode::py_nz: { // Back - Bottom
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

			case TDF::ViewMode::py_pz: { // Top - Back
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

			case TDF::ViewMode::ny_pz: { // Top - Front
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

			case TDF::ViewMode::ny_nz: { // Front - Bottom
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

			case TDF::ViewMode::nx_nz: { // Left - Bottom
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

			case TDF::ViewMode::nx_pz: { // top - Left
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

			case TDF::ViewMode::px_pz: { // Top - Right
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

			case TDF::ViewMode::px_nz: { // Right - Bottom
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

			case TDF::ViewMode::nx_py: { // Back - Left
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

			case TDF::ViewMode::px_py: { // Right - Back
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

			case TDF::ViewMode::px_ny: { // Front - Right
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

			case TDF::ViewMode::nx_ny: { // Left - Front
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

			case TDF::ViewMode::nx_py_nz: { // bottom - left - back
				cSetPosition.Set(target.x - cVertexVector.x, target.y + cVertexVector.y, -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, 0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.z, -cUpVector.x, -cUpVector.y);
				}
			} break;

			case TDF::ViewMode::nx_py_pz: { // top - back - left
				cSetPosition.Set(target.x - cVertexVector.x, target.y + cVertexVector.y, target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(-cUpVector.y, cUpVector.z, -cUpVector.x);
				}
			} break;

			case TDF::ViewMode::nx_ny_pz: { // top - left - front
				cSetPosition.Set(target.x - cVertexVector.x, -(target.y + cVertexVector.y), target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(0.408248f, 0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(-cUpVector.z, cUpVector.x, -cUpVector.y);
				}
			} break;

			case TDF::ViewMode::nx_ny_nz: { // bottom - front - left
				cSetPosition.Set(target.x - cVertexVector.x, -(target.y + cVertexVector.y), -(target.z - cVertexVector.z));
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.y, cUpVector.z, cUpVector.x);
				}
			} break;

			case TDF::ViewMode::px_py_pz: { // ISO top - right - back
				cSetPosition.Set(target.x + cVertexVector.x, target.y + cVertexVector.y, target.z - cVertexVector.z);
				if (eViewMode != eOldViewMode) {
					cSetUpVector.Set(-0.408248f, -0.408248f, 0.816497f);
				}
				else {
					cSetUpVector.Set(cUpVector.z, cUpVector.x, cUpVector.y);
				}
			} break;

			case TDF::ViewMode::px_py_nz: { // bottom - back - right
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

void TDF::BaseView::SetNavigationCube(NavigationCube * pcNaviCube)
{
	m_pcNaviCube = pcNaviCube;
}