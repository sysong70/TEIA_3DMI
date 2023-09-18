#include "StdAfx.h"

#include "Operator.CameraSelect.h"
#include "Private/SelectionPrivate.h"

#include "Window.h"
#include "Line.h"

#include "Camera.h"

#include "Operator.ObjectSnap.h"

#include <Common_Define.h>

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>
#include <HOpCameraPan.h>
#include <HBhvBehaviorManager.h>

USING_3DF_NAMESPACE

Operator::CameraSelect::CameraSelect(WindowKey * pcWindow, NavigationCube & cNaviCube, int DoRepeat, int DoCapture) :
	HOpCameraOrbit(pcWindow->GetBaseView(), DoRepeat, DoCapture),
	m_cObjectSnapOperator(pcWindow)
{
	m_pcWindow = pcWindow;

	m_pcNaviCube = &cNaviCube;

	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
	m_bOrbitMode = false;
}

/*
Operator::CameraSelect::CameraSelect(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraOrbit(view, DoRepeat, DoCapture)
{
	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
	m_bOrbitMode = false;
}
*/

Operator::CameraSelect::~CameraSelect()
{
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey());
	HC_Flush_Contents(".", "geometry");
	HC_Close_Segment();
}

const char * Operator::CameraSelect::GetName()
{
	return "TDF_Operator_CameraOrbitSelect";
}

HBaseOperator * Operator::CameraSelect::Clone()
{
	return new Operator::CameraSelect(m_pcWindow, *m_pcNaviCube);
}
//== Mouse Event 처리 ===============================================================================

int Operator::CameraSelect::OnLButtonDown(HEventInfo & cInEvent)
{
	m_cMouseDownPoint = cInEvent.GetMousePixelPos();
	m_nMouseDownTickCount = GetTickCount();
	m_bOrbitMode = false;

	// Shift & L Button 이벤트는 Area Select
	if (MVO_SHIFT & cInEvent.GetFlags()) {
	}

	return HOpCameraOrbit::OnLButtonDown(cInEvent);
}

int Operator::CameraSelect::OnLButtonUp(HEventInfo & cInEvent)
{
	if (nullptr != m_pcNaviCube) {
		if (HLISTENER_CONSUME_EVENT == m_pcNaviCube->LButtonUp(cInEvent)) {
			return HLISTENER_CONSUME_EVENT;
		}
	}

	DWORD nMouseUpTickCount = GetTickCount();
	DWORD nTickCount = nMouseUpTickCount - m_nMouseDownTickCount;

	// 2 Pixel이하 200 Tick이하에서만 선택하는 것으로 판정한다.
	if (m_nSelectPickCount > nTickCount) {
		const HPoint & cMoustPoint = cInEvent.GetMousePixelPos();
		HVector cVector = cMoustPoint - m_cMouseDownPoint;
		double dLength = HC_Compute_Vector_Length(&cVector);

		if (2.0 > dLength) {
			Point cPoint;
			cPoint.x = cInEvent.GetMouseWindowPos().x;
			cPoint.y = cInEvent.GetMouseWindowPos().y;

			H3DF::SelectionResults cResult;
			m_pcWindow->GetSelectionControl().SelectByPoint(cPoint, cInEvent.GetFlags(), cResult);
		}
	}

	m_bOrbitMode = false;

	m_cClickPoint = cInEvent.GetMouseWorldPos();

	return HOpCameraOrbit::OnLButtonUp(cInEvent);
}

int Operator::CameraSelect::OnLButtonDownAndMove(HEventInfo & cInEvent)
{
	m_bOrbitMode = true;

	if (nullptr != m_pcNaviCube) {
		m_pcNaviCube->LButtonDownAndMove(cInEvent);
	}

	int nResult = HOpCameraOrbit_OnLButtonDownAndMove(cInEvent);

	m_cObjectSnapOperator.DrawSnapItems(false);

// 	if (nullptr != m_pcNaviCube) {
// 		m_pcNaviCube->Transform();
// 	}

	GetView()->Update();

	return nResult;
}

int Operator::CameraSelect::OnRButtonDown(HEventInfo & hevent)
{
	return HOpCameraPan_OnLButtonDown(hevent);
}

int Operator::CameraSelect::OnRButtonDownAndMove(HEventInfo & hevent)
{
	return HOpCameraPan_OnLButtonDownAndMove(hevent);
}

int Operator::CameraSelect::OnRButtonUp(HEventInfo & hevent)
{
	return HOpCameraPan_OnLButtonUp(hevent);
}

// Dynamic Highlighting 처리
int Operator::CameraSelect::OnNoButtonDownAndMove(HEventInfo & cInEvent)
{
	if (nullptr != m_pcNaviCube) {
		if (HLISTENER_CONSUME_EVENT == m_pcNaviCube->NoButtonDownAndMove(cInEvent)) {
			return HLISTENER_CONSUME_EVENT;
		}
	}

	m_cObjectSnapOperator.NoButtonDownAndMove(cInEvent);

	return HLISTENER_PASS_EVENT;
}

int Operator::CameraSelect::OnMouseWheel(HEventInfo & cInEvent)
{
	int nResult = HBaseView_OnMouseWheel(cInEvent, false);

	m_cObjectSnapOperator.DrawSnapItems(true);

	return nResult;
}

int Operator::CameraSelect::HOpCameraOrbit_OnLButtonDownAndMove(HEventInfo & event)
{
	HPoint first_point, new_point, axis, vtmp, m_real_new;
	float theta, dist, tmp, vl;

	if (!OperatorStarted()) return HBaseOperator::OnLButtonDownAndMove(event);

	m_bSingleClick = false;
	GetView()->SetViewMode(HViewUnknown);
	m_pcWindow->GetBaseView()->SetViewMode(H3DF::ViewMode::Unknown);

	// read mouse position
	SetNewPoint(event.GetMouseWindowPos());

	// remember the real mouse positions
	m_real_new.x = GetNewPoint().x;
	m_real_new.y = GetNewPoint().y;

	// map screen mouse points to sphere mouse points
	tmp = GetNewPoint().x * GetNewPoint().x + GetNewPoint().y * GetNewPoint().y;
	vl = (float)sqrt(tmp);

	new_point = GetNewPoint();
	if (vl > 1.0f)
	{
		new_point.x /= vl;
		new_point.y /= vl;
		new_point.z = 0.0;
	}
	else {
		new_point.z = (float)sqrt(1.0f - tmp);
	}
	SetNewPoint(new_point);

	// get the axis of rotation
	first_point = GetFirstPoint();
	HC_Compute_Cross_Product(&first_point, &new_point, &axis);

	// this is for screen mouse based movement
	vtmp.x = m_real_new.x - m_ptRealOld.x;
	vtmp.y = m_real_new.y - m_ptRealOld.y;
	dist = (float)sqrt(vtmp.x * vtmp.x + vtmp.y * vtmp.y) * 90.0f;

	if ((axis.x != 0.0 || axis.y != 0.0 || axis.z != 0)) {

		if (GetView()->GetHandedness() == HandednessRight)
		{
			axis.y *= -1;
			axis.z *= -1;
		}

		HC_Compute_Normalized_Vector(&axis, &axis);

		HC_Open_Segment_By_Key(GetView()->GetSceneKey());

		// project axis of rotation onto yz plane 
		vtmp.x = 0.0;
		vtmp.y = axis.y;
		vtmp.z = axis.z;

		// calculate angle of x orbit
		tmp = (float)HC_Compute_Dot_Product(&axis, &vtmp);
		if (fabs(tmp) > 1.001f || fabs(tmp) < 0.999f)
			theta = (float)H_ACOS(tmp);
		else
			theta = 0.0f;


		if (axis.x < 0.0)
			m_Angle2 = -theta * dist;
		else
			m_Angle2 = theta * dist;

		// project axis of rotation onto xz plane 
		vtmp.x = axis.x;
		vtmp.y = 0.0;
		vtmp.z = axis.z;

		// calculate angle of y orbit
		tmp = (float)HC_Compute_Dot_Product(&axis, &vtmp);
		if (fabs(tmp) > 1.001f || fabs(tmp) < 0.999f)
			theta = (float)H_ACOS(tmp);
		else
			theta = 0.0f;

		if (axis.y < 0.0)
			m_Angle1 = theta * dist;
		else
			m_Angle1 = -theta * dist;

		// project axis of rotation onto xy plane 
		vtmp.x = axis.x;
		vtmp.y = axis.y;
		vtmp.z = 0.0;

		// calculate angle of z orbit
		tmp = (float)HC_Compute_Dot_Product(&axis, &vtmp);
		if (fabs(tmp) > 1.001f || fabs(tmp) < 0.999f)
			theta = (float)H_ACOS(tmp);
		else
			theta = 0.0f;

		if (axis.z < 0.0)
			m_Angle3 = theta * dist;
		else
			m_Angle3 = -theta * dist;

		HC_Orbit_Camera(m_Angle1, 0);
		HC_Orbit_Camera(0, m_Angle2);
		HC_Roll_Camera(m_Angle3);

		HC_Close_Segment();

		// update default light
		GetView()->CameraPositionChanged();

	}

	// update sphere space mouse
	SetFirstPoint(GetNewPoint());

	// update screen space mouse
	m_ptRealOld.x = m_real_new.x;
	m_ptRealOld.y = m_real_new.y;

//	GetView()->Update();
	return HOP_OK;

}

int Operator::CameraSelect::HOpCameraPan_OnLButtonDown(HEventInfo & event)
{
	if (GetView()->GetModel()->GetBhvBehaviorManager()->IsPlaying() && GetView()->GetModel()->GetBhvBehaviorManager()->GetCameraUpdated())
		return HOP_OK;

	if (!OperatorStarted())
		SetOperatorStarted(true);

	SetNewPoint(event.GetMouseWorldPos());

	SetFirstPoint(GetNewPoint());

	if (GetView()->GetModel()->GetContainsDouble())
	{
		m_dFirstPoint[0] = event.GetMousePixelPos().x;
		m_dFirstPoint[1] = event.GetMousePixelPos().y;
		m_dFirstPoint[2] = event.GetMousePixelPos().z;
		HC_Open_Segment_By_Key(GetView()->GetSceneKey());
		HC_DCompute_Coordinates(".", "local pixels", &m_dFirstPoint, "world", &m_dFirstPoint);
		HC_Close_Segment();
	}

	GetView()->PrepareForCameraChange();

	return HOP_OK;
}

int Operator::CameraSelect::HOpCameraPan_OnLButtonDownAndMove(HEventInfo & event)
{
	if (!OperatorStarted()) {
		return HBaseOperator::OnLButtonDownAndMove(event);
	}

	m_pcWindow->GetBaseView()->SetViewMode(H3DF::ViewMode::Unknown);

	SetNewPoint(event.GetMouseWorldPos());

	HC_Open_Segment_By_Key(GetView()->GetSceneKey());
	bool const set_default_camera = HC_Show_Existence("camera") == 0;

	if (GetView()->GetModel()->GetContainsDouble())
	{
		if (set_default_camera)
			HC_DSet_Camera_Target(0, 0, 0);

		double dpoint[3] = {
			event.GetMousePixelPos().x,
			event.GetMousePixelPos().y,
			event.GetMousePixelPos().z
		};
		HC_DCompute_Coordinates(".", "local pixels", &dpoint, "world", &dpoint);

		double const delta[3] = {
			dpoint[0] - m_dFirstPoint[0],
			dpoint[1] - m_dFirstPoint[1],
			dpoint[2] - m_dFirstPoint[2]
		};

		double camera[3], target[3];
		HC_DShow_Camera_Target(&target[0], &target[1], &target[2]);
		HC_DShow_Camera_Position(&camera[0], &camera[1], &camera[2]);

		HC_DSet_Camera_Target(target[0] - delta[0], target[1] - delta[1], target[2] - delta[2]);
		HC_DSet_Camera_Position(camera[0] - delta[0], camera[1] - delta[1], camera[2] - delta[2]);
	}
	else
	{
		if (set_default_camera)
			HC_Set_Camera_Target(0, 0, 0);

		HPoint const delta(GetNewPoint() - GetFirstPoint());

		HPoint camera, target;
		HC_Show_Camera_Target(&target.x, &target.y, &target.z);
		HC_Show_Camera_Position(&camera.x, &camera.y, &camera.z);

		HC_Set_Camera_Target(target.x - delta.x, target.y - delta.y, target.z - delta.z);
		HC_Set_Camera_Position(camera.x - delta.x, camera.y - delta.y, camera.z - delta.z);
	}

	HC_Close_Segment();

	GetView()->CameraPositionChanged();

	GetView()->Update();
	return HOP_OK;
}
int Operator::CameraSelect::HOpCameraPan_OnLButtonUp(HEventInfo & event)
{
	if (!OperatorStarted())
		return HBaseOperator::OnLButtonDownAndMove(event);

	SetOperatorStarted(false);
	GetView()->CameraPositionChanged(true, true);

	return HOP_READY;
}


int Operator::CameraSelect::HBaseView_OnMouseWheel(HEventInfo & event, bool bUdpate)
{
	float zDelta = static_cast<float>(event.GetMouseWheelDelta() / m_pcWindow->GetBaseView()->GetMouseWheelSensitivity() / 120.0 / 8.0);

	if (m_pcWindow->GetBaseView()->GetInvertMouseWheelZoom()) {
		zDelta *= -1;
	}

	m_pcWindow->GetBaseView()->SetViewMode(H3DF::ViewMode::Unknown);

	HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetSceneKey());

	/* Save the original camera for smooth transition. */
	HCamera orig;
	HC_Show_Net_Camera(&orig.position, &orig.target, &orig.up_vector, &orig.field_width, &orig.field_height, orig.projection);

	/* We need to fill in these values with the new camera. */
	HCamera adjusted = orig;

	if (event.Shift()) {
		if (zDelta >= 0) {
			adjusted.field_width *= 2.0f;
			adjusted.field_height *= 2.0f;
		}
		else {
			adjusted.field_width *= 0.5f;
			adjusted.field_height *= 0.5f;
		}
	}
	else {

		if (!event.Control()) {
			m_pcWindow->GetBaseView()->ComputeReasonableTarget(adjusted.target, event.GetMouseWindowPos(), orig.target);
			if (streq(orig.projection, "perspective"))
				m_pcWindow->GetBaseView()->ComputeNewField(adjusted.field_width, adjusted.field_height, adjusted.target, orig);
			adjusted.field_width *= fabs(1 + zDelta);
			adjusted.field_height *= fabs(1 + zDelta);
		}
		else {
			HVector const to_target = orig.target - orig.position;
			adjusted.target = orig.position + to_target * (1 - zDelta);
			if (streq(orig.projection, "orthographic")) {
				adjusted.field_width *= fabs(1 + zDelta);
				adjusted.field_height *= fabs(1 + zDelta);
			}
		}

		double	position_scale = HC_Compute_Vector_Length(&orig.position);
		double	target_scale = HC_Compute_Vector_Length(&orig.target);
		double	camera_scale = MAX(position_scale, target_scale);

		double	diagonal_len = sqrt(pow(adjusted.field_width, 2) + pow(adjusted.field_height, 2));

		/* If the camera is about to be too small... */
		if (diagonal_len < m_pcWindow->GetBaseView()->GetZoomLimit() || diagonal_len / camera_scale < 1.0e-6) {
			if (zDelta < 0) {
				goto BAILOUT;
			}
			adjusted.field_width = orig.field_width * fabs(1 + zDelta);
			adjusted.field_height = orig.field_height * fabs(1 + zDelta);
			diagonal_len = sqrt(pow(adjusted.field_width, 2) + pow(adjusted.field_height, 2));
			adjusted.target = orig.target;
		}
		/* If the camera is about to be too big... */
		if (diagonal_len > fabs(MVO_SQRT_MAX_FLOAT))
			goto BAILOUT;

		/* Shift the target slightly toward the mouse pointer. */
		HVector mwp = event.GetMouseWorldPos();
		HUtility::AdjustPositionToPlane(m_pcWindow->GetBaseView(), mwp, adjusted.target);
		adjusted.target += (adjusted.target - mwp) * zDelta;

		HVector dir_to_position = orig.position - orig.target;
		double	old_diagonal_len = sqrt(pow(orig.field_width, 2) + pow(orig.field_height, 2));
		double	old_ratio = HC_Compute_Vector_Length(&dir_to_position) / old_diagonal_len;
		HC_Compute_Normalized_Vector(&dir_to_position, &dir_to_position);
		adjusted.position = adjusted.target + dir_to_position * static_cast<float>(old_ratio * diagonal_len);
	}

	if (valid_point(adjusted.position) &&
		valid_point(adjusted.target) &&
		valid_point(adjusted.up_vector) &&
		valid_float(adjusted.field_width) &&
		valid_float(adjusted.field_height)) {
		if (!adjusted.position.Equal(adjusted.target, m_pcWindow->GetBaseView()->GetZoomLimit())) {
			m_pcWindow->GetBaseView()->PrepareForCameraChange(); {
				HC_Set_Camera_Position(adjusted.position.x, adjusted.position.y, adjusted.position.z);
				HC_Set_Camera_Target(adjusted.target.x, adjusted.target.y, adjusted.target.z);
				HC_Set_Camera_Field(adjusted.field_width, adjusted.field_height);
			}m_pcWindow->GetBaseView()->CameraPositionChanged(true, false);

			if (m_pcWindow->GetBaseView()->GetModel()->GetContainsDouble())
				HC_Convert_Precision(m_pcWindow->GetBaseView()->GetSceneKey(), "double, camera");

			if (true == bUdpate) {
				m_pcWindow->Update();
			}

			if (m_pcWindow->GetBaseView()->GetHandleOperator())
				m_pcWindow->GetBaseView()->GetHandleOperator()->OnNoButtonDownAndMove(event);
		}
	}

BAILOUT:

	HC_Close_Segment();

	return HLISTENER_CONSUME_EVENT;
}

bool Operator::CameraSelect::valid_float(float f)
{
	if (isinf(f) || isnan(f)) {
		return false;
	}
	return true;
}

bool Operator::CameraSelect::valid_point(HPoint const & p)
{
	if (valid_float(p.x) && valid_float(p.y) && valid_float(p.z)) {
		return true;
	}
	return false;
}
