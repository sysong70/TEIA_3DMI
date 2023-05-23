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

void TDF::BaseView::SetViewMode(TDF::ViewMode eViewMode, bool invert, bool fitWorld)
{
	HPoint target, camera, view;
	float length;

	HPoint cPosition, cTarget, cUpVector;
	float widtho, heighto;
	char projection[MVO_BUFFER_SIZE];

// 	if (eViewMode == HViewIso) {
// 		eViewMode = GetIsoDefault();
// 	}

	HC_Open_Segment_By_Key(GetSceneKey()); {
		HC_PShow_Net_Camera(0, 0, &cPosition, &cTarget, &cUpVector, &widtho, &heighto, projection);
	}HC_Close_Segment();

	TDF::ViewMode OldViewMode = m_eViewMode;
	m_eViewMode = eViewMode;

// 	if (eViewMode == HViewUnknown) 
// 		return;

	PrepareForCameraChange();

	if (fitWorld) {
		FitWorld();
	}
	
	HC_Open_Segment_By_Key(GetSceneKey());

	HC_PShow_Net_Camera_Target(0, 0, &target.x, &target.y, &target.z);
	HC_PShow_Net_Camera_Position(0, 0, &camera.x, &camera.y, &camera.z);

	view.Set(camera.x - target.x, camera.y - target.y, camera.z - target.z);

	length = (float)HC_Compute_Vector_Length(&view);

/*
	if (m_eViewMode == OldViewMode && invert)
	{
		if (m_eViewMode == HViewXY)
			m_eViewMode = HViewYX;
		else if (m_eViewMode == HViewYX)
			m_eViewMode = HViewXY;
		else if (m_eViewMode == HViewXZ)
			m_eViewMode = HViewZX;
		else if (m_eViewMode == HViewZX)
			m_eViewMode = HViewXZ;
		else if (m_eViewMode == HViewYZ)
			m_eViewMode = HViewZY;
		else if (m_eViewMode == HViewZY)
			m_eViewMode = HViewYZ;

		if (m_eViewMode == HViewFront)
			m_eViewMode = HViewBack;
		else if (m_eViewMode == HViewBack)
			m_eViewMode = HViewFront;
		else if (m_eViewMode == HViewLeft)
			m_eViewMode = HViewRight;
		else if (m_eViewMode == HViewRight)
			m_eViewMode = HViewLeft;
		else if (m_eViewMode == HViewTop)
			m_eViewMode = HViewBottom;
		else if (m_eViewMode == HViewBottom)
			m_eViewMode = HViewTop;
	}
*/

	float newLen = length * 0.5774f;

	HPoint rightaxis;
	HC_Compute_Cross_Product(&m_FrontAxis, &m_TopAxis, &rightaxis);

	if (GetHandedness() == HandednessRight) {
		rightaxis.Set(-rightaxis.x, -rightaxis.y, -rightaxis.z);
	}

	float px = target.x + newLen * m_FrontAxis.x - newLen * rightaxis.x + newLen * m_TopAxis.x;
	float py = target.y + newLen * m_FrontAxis.y - newLen * rightaxis.y + newLen * m_TopAxis.y;
	float pz = target.z + newLen * m_FrontAxis.z - newLen * rightaxis.z + newLen * m_TopAxis.z;

	float cos45 = cos(M_PI / 4);

	switch (m_eViewMode) {

		case TDF::ViewMode::right: {
			HC_Set_Camera_Position(target.x + length * m_FrontAxis.x, target.y + length * m_FrontAxis.y, target.z + length * m_FrontAxis.z);
			HC_Set_Camera_Up_Vector(0, 0, 1);
		} break;

		case TDF::ViewMode::left: {
			HC_Set_Camera_Position(target.x - length * m_FrontAxis.x, target.y + length * m_FrontAxis.y, target.z + length * m_FrontAxis.z);
			HC_Set_Camera_Up_Vector(0, 0, 1);
		} break;

		case TDF::ViewMode::front: {
			HC_Set_Camera_Position(target.x + length * m_TopAxis.x, target.y - length * m_TopAxis.y, target.z + length * m_TopAxis.z);
			HC_Set_Camera_Up_Vector(0, 0, 1);
		} break;

		case TDF::ViewMode::back: {
			HC_Set_Camera_Position(target.x + length * m_TopAxis.x, target.y + length * m_TopAxis.y, target.z + length * m_TopAxis.z);
			HC_Set_Camera_Up_Vector(0, 0, 1);
		} break;

		case TDF::ViewMode::top: {
			HC_Set_Camera_Position(target.x + length * rightaxis.x, target.y + length * rightaxis.y, target.z - length * rightaxis.z);
			HC_Set_Camera_Up_Vector(0, 1, 0);
		} break;

		case TDF::ViewMode::bottom: {
			HC_Set_Camera_Position(target.x + length * rightaxis.x, target.y + length * rightaxis.y, target.z + length * rightaxis.z);
			HC_Set_Camera_Up_Vector(0, 1, 0);
		} break;

		case TDF::ViewMode::py_nz: {
			HC_Set_Camera_Position(target.x, target.y + length * cos45, target.z - length * cos45);
			HC_Set_Camera_Up_Vector(0, cos45, cos45);
		} break;

		case TDF::ViewMode::py_pz: {
			HC_Set_Camera_Position(target.x, target.y + length * cos45, target.z + length * cos45);
			HC_Set_Camera_Up_Vector(0, -cos45, cos45);
		} break;

		case TDF::ViewMode::ny_pz: {
			HC_Set_Camera_Position(target.x, target.y - length * cos45, target.z + length * cos45);
			HC_Set_Camera_Up_Vector(0, cos45, cos45);
		} break;

		case TDF::ViewMode::ny_nz: {
			HC_Set_Camera_Position(target.x, target.y - length * cos45, target.z - length * cos45);
			HC_Set_Camera_Up_Vector(0, -cos45, cos45);
		} break;

		case TDF::ViewMode::nx_nz: {
			HC_Set_Camera_Position(target.x - length * cos45, target.y, target.z - length * cos45);
			HC_Set_Camera_Up_Vector(-cos45, 0, cos45);
		} break;

		case TDF::ViewMode::nx_pz: {
			HC_Set_Camera_Position(target.x - length * cos45, target.y, target.z + length * cos45);
			HC_Set_Camera_Up_Vector(cos45, 0, cos45);
		} break;

		case TDF::ViewMode::px_pz: {
			HC_Set_Camera_Position(target.x + length * cos45, target.y, target.z + length * cos45);
			HC_Set_Camera_Up_Vector(-cos45, 0, cos45);
		} break;

		case TDF::ViewMode::px_nz: {
			HC_Set_Camera_Position(target.x + length * cos45, target.y, target.z - length * cos45);
			HC_Set_Camera_Up_Vector(cos45, 0, cos45);
		} break;

		case TDF::ViewMode::nx_py: {
			HC_Set_Camera_Position(target.x - length * cos45, target.y + length * cos45, target.z);
			HC_Set_Camera_Up_Vector(0, 0, 1);
		} break;

		case TDF::ViewMode::px_py: {
			HC_Set_Camera_Position(target.x + length * cos45, target.y + length * cos45, target.z);
			HC_Set_Camera_Up_Vector(0, 0, 1);
		} break;

		case TDF::ViewMode::px_ny: {
			HC_Set_Camera_Position(target.x + length * cos45, target.y - length * cos45, target.z);
			HC_Set_Camera_Up_Vector(0, 0, 1);
		} break;

		case TDF::ViewMode::nx_ny: {
			HC_Set_Camera_Position(target.x - length * cos45, target.y - length * cos45, target.z);
			HC_Set_Camera_Up_Vector(0, 0, 1);
		} break;

		case TDF::ViewMode::nx_py_nz:
		{
			HC_Set_Camera_Position(-px, py, -pz);
			HC_Set_Camera_Up_Vector(-0.408248f, 0.408249f, 0.816497f);
		}
		break;

		case TDF::ViewMode::nx_py_pz:
		{
			HC_Set_Camera_Position(-px, py, pz);
			HC_Set_Camera_Up_Vector(0.408248f, -0.408249f, 0.816497f);
		}
		break;

		case TDF::ViewMode::nx_ny_pz:
		{
			HC_Set_Camera_Position(-px, -py, pz);
			HC_Set_Camera_Up_Vector(0.408248f, 0.408249f, 0.816497f);
		}
		break;

		case TDF::ViewMode::nx_ny_nz:
		{
			HC_Set_Camera_Position(-px, -py, -pz);
			HC_Set_Camera_Up_Vector(-0.408248f, -0.408248f, 0.816497f);
		}
		break;

		case TDF::ViewMode::px_py_pz: // ISO
		{
			HC_Set_Camera_Position(px, py, pz);
			HC_Set_Camera_Up_Vector(-0.408248f, -0.408249f, 0.816497f);
		}
		break;

		case TDF::ViewMode::px_py_nz:
		{
			HC_Set_Camera_Position(px, py, -pz);
			HC_Set_Camera_Up_Vector(0.468564f, 0.317602f, 0.824364f);
		}
		break;

		case ViewMode::px_ny_nz:
		{
			HC_Set_Camera_Position(px, -py, -pz);
			HC_Set_Camera_Up_Vector(0.468564f, -0.317602f, 0.824364f);
		}
		break;

		case ViewMode::px_ny_pz:
		{
			HC_Set_Camera_Position(px, -py, pz);
			HC_Set_Camera_Up_Vector(-0.408249f, 0.408248f, 0.816497f);
		}
		break;

/*
		case HViewIsoFrontLeftTop:
		case HViewSWIso:
		{
			camera.Set(target.x + newLen * m_FrontAxis.x - newLen * rightaxis.x + newLen * m_TopAxis.x,
				target.y + newLen * m_FrontAxis.y - newLen * rightaxis.y + newLen * m_TopAxis.y,
				target.z + newLen * m_FrontAxis.z - newLen * rightaxis.z + newLen * m_TopAxis.z);
			HC_Set_Camera_Position(camera.x, camera.y, camera.z);
			HC_Set_Camera_Up_Vector(m_TopAxis.x, m_TopAxis.y, m_TopAxis.z);
		}
		break;
		case HViewIsoFrontLeftBottom:
		{
			camera.Set(target.x + newLen * m_FrontAxis.x -
				newLen * rightaxis.x - newLen * m_TopAxis.x,
				target.y + newLen * m_FrontAxis.y -
				newLen * rightaxis.y - newLen * m_TopAxis.y,
				target.z + newLen * m_FrontAxis.z -
				newLen * rightaxis.z - newLen * m_TopAxis.z);
			HC_Set_Camera_Position(camera.x, camera.y, camera.z);
			HC_Set_Camera_Up_Vector(m_TopAxis.x, m_TopAxis.y, m_TopAxis.z);
		}
		break;
		case HViewIsoFrontRightBottom:
		{
			camera.Set(target.x + newLen * m_FrontAxis.x +
				newLen * rightaxis.x - newLen * m_TopAxis.x,
				target.y + newLen * m_FrontAxis.y +
				newLen * rightaxis.y - newLen * m_TopAxis.y,
				target.z + newLen * m_FrontAxis.z +
				newLen * rightaxis.z - newLen * m_TopAxis.z);
			HC_Set_Camera_Position(camera.x, camera.y, camera.z);
			HC_Set_Camera_Up_Vector(m_TopAxis.x, m_TopAxis.y, m_TopAxis.z);
		}
		break;
		case HViewIsoBackRightTop:
		case HViewNEIso:
		{
			camera.Set(target.x - newLen * m_FrontAxis.x +
				newLen * rightaxis.x + newLen * m_TopAxis.x,
				target.y - newLen * m_FrontAxis.y + newLen * rightaxis.y + newLen * m_TopAxis.y,
				target.z - newLen * m_FrontAxis.z +	newLen * rightaxis.z + newLen * m_TopAxis.z);
			HC_Set_Camera_Position(camera.x, camera.y, camera.z);
			HC_Set_Camera_Up_Vector(m_TopAxis.x, m_TopAxis.y, m_TopAxis.z);
		}
		break;
		case HViewIsoBackLeftTop:
		case HViewNWIso:
		{
			camera.Set(target.x - newLen * m_FrontAxis.x -
				newLen * rightaxis.x + newLen * m_TopAxis.x,
				target.y - newLen * m_FrontAxis.y -
				newLen * rightaxis.y + newLen * m_TopAxis.y,
				target.z - newLen * m_FrontAxis.z -
				newLen * rightaxis.z + newLen * m_TopAxis.z);
			HC_Set_Camera_Position(camera.x, camera.y, camera.z);
			HC_Set_Camera_Up_Vector(m_TopAxis.x, m_TopAxis.y, m_TopAxis.z);
		}
		break;
		case HViewIsoBackLeftBottom:
		{
			camera.Set(target.x - newLen * m_FrontAxis.x -
				newLen * rightaxis.x - newLen * m_TopAxis.x,
				target.y - newLen * m_FrontAxis.y -
				newLen * rightaxis.y - newLen * m_TopAxis.y,
				target.z - newLen * m_FrontAxis.z -
				newLen * rightaxis.z - newLen * m_TopAxis.z);
			HC_Set_Camera_Position(camera.x, camera.y, camera.z);
			HC_Set_Camera_Up_Vector(m_TopAxis.x, m_TopAxis.y, m_TopAxis.z);
		}
		break;
		case HViewIsoBackRightBottom:
		{
			camera.Set(target.x - newLen * m_FrontAxis.x +
				newLen * rightaxis.x - newLen * m_TopAxis.x,
				target.y - newLen * m_FrontAxis.y +
				newLen * rightaxis.y - newLen * m_TopAxis.y,
				target.z - newLen * m_FrontAxis.z +
				newLen * rightaxis.z - newLen * m_TopAxis.z);
			HC_Set_Camera_Position(camera.x, camera.y, camera.z);
			HC_Set_Camera_Up_Vector(m_TopAxis.x, m_TopAxis.y, m_TopAxis.z);
		}
		break;
*/

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

	HC_Close_Segment();
}

void TDF::BaseView::SetNavigationCube(NavigationCube * pcNaviCube)
{
	m_pcNaviCube = pcNaviCube;
}