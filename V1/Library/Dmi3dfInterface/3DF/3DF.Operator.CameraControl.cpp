#include "StdAfx.h"

#include "3DF.Operator.CameraControl.h"
#include "Impl/SelectionImpl.h"

#include "Impl/OperatorImpl.h"

#include "Window.h"
#include "Line.h"

#include "Camera.h"

#include "../Impl/ViewImpl.h"

#include <Common_Define.h>

#include <HBaseView.h>
#include <HTools.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HConstantFrameRate.h>
#include <HBhvBehaviorManager.h>

#include <HOpCameraOrbit.h>
#include <HOpCameraOrbitTurntable.h>
#include <HOpCameraPan.h>
#include <HOpCameraZoomBox.h>

#include <float.h>

#define isinf(x) (!_finite(x))
#define isnan(x) _isnan(x)

static bool valid_float(float f)
{
	if (isinf(f) || isnan(f)) {
		return false;
	}
	return true;
}

static bool valid_point(HPoint const & p)
{
	if (valid_float(p.x) && valid_float(p.y) && valid_float(p.z)) {
		return true;
	}
	return false;
}

class CameraZoomBox : public HOpCameraZoomBox
{
public:
	CameraZoomBox(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);
	int OnLButtonUp(HEventInfo & hevent) override;
};

class CameraOrbitTurntable : public HOpCameraOrbitTurntable
{
public:
	CameraOrbitTurntable(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);
	int OnLButtonDownAndMove(HEventInfo & hevent) override;
};

CameraZoomBox::CameraZoomBox(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraZoomBox(view, DoRepeat, DoCapture)
{

}

#define MINIMUM_FIELD_SIZE (1e-9f)

// diagonal_len 계산 오류로 인해서 상속해서 사용함.
int CameraZoomBox::OnLButtonUp(HEventInfo & cInEvent)
{
	HOpConstructRectangle::OnLButtonUp(cInEvent);

	if(GetView()->GetModel()->GetBhvBehaviorManager()->IsPlaying() &&
		GetView()->GetModel()->GetBhvBehaviorManager()->GetCameraUpdated())
		return HOP_OK;

	if(!m_bRectangleExists)
		return HBaseOperator::OnLButtonUp(cInEvent);

	HUtility::Order(&m_ptRectangle[0], &m_ptRectangle[1]);
	HUtility::ClampPointToWindow(&m_ptRectangle[0]);
	HUtility::ClampPointToWindow(&m_ptRectangle[1]);

	if(m_ptRectangle[1].x - m_ptRectangle[0].x < MINIMUM_FIELD_SIZE ||
		m_ptRectangle[1].y - m_ptRectangle[0].y < MINIMUM_FIELD_SIZE)
		return HOP_OK;

	HC_Open_Segment_By_Key(GetView()->GetSceneKey());
	{
		/* Save the original camera for smooth transition. */
		HCamera orig;
		HC_Show_Net_Camera(&orig.position, &orig.target, &orig.up_vector, &orig.field_width, &orig.field_height, orig.projection);

		/* We need to fill in these values with the new camera. */
		HCamera cAdjustedCamera = orig;

		// Compute a Selection_By_Area using the Rectangle to find the object closest to the viewer

		// THIS IS NOW DONE IN ComputeReasonableTarget
		// Save selection settings if locally set
		// 		char heur_sav[4096] = {""};
		// 		char selectability_sav[4096] = {""};
		//
		// 		if (HC_Show_Existence("selectability")) {
		// 			HC_Show_Selectability (selectability_sav);
		// 		}
		//
		// 		if (HC_Show_Existence("heuristics")) {
		// 			HC_Show_Heuristics(heur_sav);
		// 		}

		// 		HC_Set_Heuristics ("related selection limit = 0, internal selection limit=-1");
		// 		HC_Set_Selectability ("geometry=on");

	bool anything_selected = ComputeReasonableTarget(cAdjustedCamera.target, m_ptRectangle[0], m_ptRectangle[1], orig.target);

		if(anything_selected) {
			ComputeNewField(cAdjustedCamera.field_width, cAdjustedCamera.field_height, m_ptRectangle[0], m_ptRectangle[1], cAdjustedCamera.target);

			// #Error 3DF: 아래식을 이용해서 값을 계산하면 diagonal_len이 너무 큰값이 나와서 View가 이상해짐.
			//float diagonal_len = static_cast<float>(sqrt(pow(cAdjustedCamera.field_width, 2)) + pow(cAdjustedCamera.field_height, 2));
			double diagonal_len = sqrt(pow(cAdjustedCamera.field_width, 2) + pow(cAdjustedCamera.field_height, 2));

			HVector viewingVector = orig.position - orig.target;
			HC_Compute_Normalized_Vector(&viewingVector, &viewingVector);
			cAdjustedCamera.position = cAdjustedCamera.target + viewingVector * 2.5 * diagonal_len;

			if(m_enforceMinCameraSize) {
				double const camera_dist = HC_Compute_Vector_Length(&cAdjustedCamera.target) + 1;
				double const min_camera = 0.0005 * camera_dist;

				/* If the camera is about to be too small... */
				if(diagonal_len < min_camera) {
					double x = min_camera / diagonal_len;
					cAdjustedCamera.field_width *= static_cast<float>(x);
					cAdjustedCamera.field_height *= static_cast<float>(x);
					diagonal_len = static_cast<float>(sqrt(pow(cAdjustedCamera.field_width, 2)) +
						static_cast<float>(pow(cAdjustedCamera.field_height, 2)));
					HVector dir_to_position = orig.position - orig.target;
					HC_Compute_Normalized_Vector(&dir_to_position, &dir_to_position);
					cAdjustedCamera.target = cAdjustedCamera.position - dir_to_position * 2.5 * diagonal_len;
				}
			}

			if(orig.CameraDifferent(cAdjustedCamera)) {
				GetView()->PrepareForCameraChange();
				if(GetView()->GetSmoothTransition()) {
					HUtility::SmoothTransition(orig.position,
						orig.target,
						orig.up_vector,
						orig.field_width,
						orig.field_height,
						cAdjustedCamera.position,
						cAdjustedCamera.target,
						cAdjustedCamera.up_vector,
						cAdjustedCamera.field_width,
						cAdjustedCamera.field_height,
						GetView());
			}
				else {
					HC_Set_Camera_Position(cAdjustedCamera.position.x, cAdjustedCamera.position.y, cAdjustedCamera.position.z);
					HC_Set_Camera_Target(cAdjustedCamera.target.x, cAdjustedCamera.target.y, cAdjustedCamera.target.z);
					HC_Set_Camera_Field(cAdjustedCamera.field_width, cAdjustedCamera.field_height);
				}
				GetView()->CameraPositionChanged(true, GetView()->GetSmoothTransition());
			}
		}

		// 		HC_UnSet_Heuristics();
		//
		// 		if (!streq(heur_sav,""))
		// 			HC_Set_Heuristics(heur_sav);
		//
		// 		HC_UnSet_Selectability ();
		//
		// 		if (!streq(selectability_sav,""))
		// 			HC_Set_Selectability (selectability_sav);
	}
	HC_Close_Segment();

	GetView()->Update();

	return HLISTENER_CONSUME_EVENT;
	//return HOP_READY;
}

CameraOrbitTurntable::CameraOrbitTurntable(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraOrbitTurntable(view, DoRepeat, DoCapture)
{

}

int CameraOrbitTurntable::OnLButtonDownAndMove(HEventInfo & cInEvent)
{
	if (!OperatorStarted())
		return HBaseOperator::OnLButtonDownAndMove(cInEvent);

	SetNewPoint(cInEvent.GetMouseWindowPos());

	HPoint delta2(GetNewPoint() - GetFirstPoint());

	// Z축 방향으로 회전하도록 수정함.
	HVector fa(1, 0, 0), ta(0, 0, 1);

	// 지금 구성되어 있는 방식으로는 Y축 방향으로 회전이 됨.
	// m_pView->GetViewAxis(&fa, &ta);

	RotateAroundAxis(ta, -delta2.x * 250.0f);

	GetView()->CameraPositionChanged();
	SetFirstPoint(GetNewPoint());

	GetView()->Update();
	return HOP_OK;
}

using namespace H3DF;

//== CameraSelectImpl Class ========================================================================
namespace H3DF
{
	namespace Operator
	{
		class CameraControlImpl : public OperatorImpl
		{
		public:
			CameraControlImpl(WindowKey const & cInWindow, NavigationCube const & cNaviCube);

			void Copy(CameraControlImpl * pcInThat) {
				OperatorImpl::Copy(pcInThat);
				m_pcNaviCube = pcInThat->m_pcNaviCube;
			}

			H3DF::Camera::Mode CameraMode() { return m_eCameraMode; }
			void SetCameraMode(H3DF::Camera::Mode eMode) { m_eCameraMode = eMode; }

			bool ComputeReasonableTarget(HPoint & new_tar, HPoint const & mouse_win, HPoint const & tar_orig);
			
			DWORD MouseMapFlags(DWORD nState);

			NavigationCube * m_pcNaviCube = nullptr;

			H3DF::Camera::Mode m_eCameraMode = H3DF::Camera::Mode::Multi;

			DWORD m_nSelectPickCount;
			DWORD m_nMouseDownTickCount;
			
			HPoint m_cMouseDownPoint;
			HPoint m_cClickPoint;

			double  m_dFirstPoint[3];

			HOpCameraOrbit m_cCameraOrbit;
			CameraOrbitTurntable m_cCameraOrbitTurntable;
			HOpCameraPan m_cCameraPan;
			CameraZoomBox m_cCameraZoomBox;
		};
	}
}

H3DF::Operator::CameraControlImpl::CameraControlImpl(WindowKey const & cInWindow, NavigationCube const & cNaviCube)
	: OperatorImpl(cInWindow),
	m_cCameraOrbit((HBaseView *)cInWindow.GetBaseView()),
	m_cCameraOrbitTurntable((HBaseView *)cInWindow.GetBaseView()),
	m_cCameraPan((HBaseView *)cInWindow.GetBaseView()),
	m_cCameraZoomBox((HBaseView *)cInWindow.GetBaseView())
{
	m_pcNaviCube = const_cast<NavigationCube *>(&cNaviCube);

	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
}

bool H3DF::Operator::CameraControlImpl::ComputeReasonableTarget(HPoint & cNewTarget, HPoint const & cWindowMousePosition, HPoint const & cOriginTarget)
{
	char chSelectabilitySave[4096] = { "" };

	if (HC_Show_Existence("selectability")) {
		HC_Show_Selectability(chSelectabilitySave);
	}

	HC_Set_Selectability("geometry=on");

	bool const selected = HC_Compute_Selection("..",
		".",
		"v, selection proximity = 0.0, related selection limit = 0, internal selection limit = 0, "
		"selection level = entity, visual selection = off",
		cWindowMousePosition.x,
		cWindowMousePosition.y) != 0;

// 	bool const selected = HC_Compute_Selection("..",
// 		".",
// 		"v, selection proximity=0.1, related selection limit = -1, internal selection "
// 		"limit=-1, selection level = entity, visual selection = off",
// 		mouse_win.x,
// 		mouse_win.y) != 0;

	HC_UnSet_Selectability();

	if (chSelectabilitySave[0] != '\0')
		HC_Set_Selectability(chSelectabilitySave);

	if (!selected)
		return false;

	HPoint cSelectionPosition;
	HC_Show_Selection_Position(0, 0, 0, &cSelectionPosition.x, &cSelectionPosition.y, &cSelectionPosition.z);
	HPoint cLocalNewTarget = cOriginTarget;
	HUtility::AdjustPositionToPlane(GetBaseView(), cLocalNewTarget, cSelectionPosition);

	cNewTarget = cLocalNewTarget;

	return true;
}

DWORD H3DF::Operator::CameraControlImpl::MouseMapFlags(DWORD nState)
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

//== CameraSelect Class =============================================================================

H3DF::Operator::CameraControl::CameraControl(WindowKey const & cInWindow, NavigationCube & cNaviCube)
{
	CameraControlImpl * pcImpl = new CameraControlImpl(cInWindow, cNaviCube);
	m_pcImpl = pcImpl;
}

H3DF::Operator::CameraControl::~CameraControl()
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	HC_Open_Segment_By_Key(pcImpl->GetBaseView()->GetConstructionKey()); {
		HC_Flush_Contents(".", "geometry");
	} HC_Close_Segment();
}

H3DF::Camera::Mode H3DF::Operator::CameraControl::CameraMode()
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->CameraMode();

}

void H3DF::Operator::CameraControl::SetCameraMode(H3DF::Camera::Mode eMode)
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->SetCameraMode(eMode);
}

void H3DF::Operator::CameraControl::FitWorld()
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->GetBaseView()->ZoomToExtents();
}

//== Mouse cInEvent 처리 ===============================================================================

// 1. Left Button Down 처리
int H3DF::Operator::CameraControl::LButtonDown(HEventInfo & cInEvent)
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->m_cMouseDownPoint = cInEvent.GetMousePixelPos();
	pcImpl->m_nMouseDownTickCount = GetTickCount();

	// Shift & L Button 이벤트는 Area Select
	if (MVO_SHIFT & cInEvent.GetFlags()) {
	}

	switch (pcImpl->m_eCameraMode)
	{
		case Camera::Mode::OrbitTurntable:
			return pcImpl->m_cCameraOrbitTurntable.OnLButtonDown(cInEvent);
			break;

		case Camera::Mode::Pan:
			return pcImpl->m_cCameraPan.OnLButtonDown(cInEvent);
			break;

		case Camera::Mode::ZoomBox:
			return pcImpl->m_cCameraZoomBox.OnLButtonDown(cInEvent);
			break;
	}

	return pcImpl->m_cCameraOrbit.OnLButtonDown(cInEvent);
}

// 2. Left Button Up 처리
// L Button Up을 핱때 Objet를 선택함.
int H3DF::Operator::CameraControl::LButtonUp(HEventInfo & cInEvent)
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	// Nvigation Cube가 있으면 Navigation Cube의 이벤트를 처리함.
	if (nullptr != pcImpl->m_pcNaviCube) {
		if (HLISTENER_CONSUME_EVENT == pcImpl->m_pcNaviCube->LButtonUp(cInEvent)) {
			return HLISTENER_CONSUME_EVENT;
		}
	}

	switch (pcImpl->m_eCameraMode)
	{
		case Camera::Mode::OrbitTurntable:
			return pcImpl->m_cCameraOrbitTurntable.OnLButtonUp(cInEvent);
			break;

		case Camera::Mode::Pan:
			return pcImpl->m_cCameraPan.OnLButtonUp(cInEvent);
			break;

		case Camera::Mode::ZoomBox:
			return pcImpl->m_cCameraZoomBox.OnLButtonUp(cInEvent);
			break;
	}

	return pcImpl->m_cCameraOrbit.OnLButtonUp(cInEvent);
}

int H3DF::Operator::CameraControl::LButtonDownAndMove(HEventInfo & cInEvent)
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	pcImpl->GetBaseView()->SetSuppressUpdate(true);

	if (nullptr != pcImpl->m_pcNaviCube) {
		pcImpl->m_pcNaviCube->LButtonDownAndMove(cInEvent);
		pcImpl->m_pcNaviCube->Transform();
	}

	int nResult = 0;

	switch (pcImpl->m_eCameraMode)
	{
		case Camera::Mode::OrbitTurntable:
			nResult = pcImpl->m_cCameraOrbitTurntable.OnLButtonDownAndMove(cInEvent);
			break;

		case Camera::Mode::Pan:
			nResult = pcImpl->m_cCameraPan.OnLButtonDownAndMove(cInEvent);
			break;

		case Camera::Mode::ZoomBox:
			nResult = pcImpl->m_cCameraZoomBox.OnLButtonDownAndMove(cInEvent);
			break;

		default:
			nResult = pcImpl->m_cCameraOrbit.OnLButtonDownAndMove(cInEvent);
			break;
	}

	pcImpl->GetBaseView()->SetSuppressUpdate(false);

	pcImpl->GetBaseView()->Update();

	return nResult;
}

int H3DF::Operator::CameraControl::RButtonDown(HEventInfo & cInEvent)
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cCameraPan.OnLButtonDown(cInEvent);
}

int H3DF::Operator::CameraControl::RButtonUp(HEventInfo & cInEvent)
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cCameraPan.OnLButtonUp(cInEvent);
}

int H3DF::Operator::CameraControl::RButtonDownAndMove(HEventInfo & cInEvent)
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	return pcImpl->m_cCameraPan.OnLButtonDownAndMove(cInEvent);
}

int H3DF::Operator::CameraControl::MouseWheel(HEventInfo & cInEvent)
{
	CameraControlImpl * pcImpl = dynamic_cast<CameraControlImpl *>(m_pcImpl);
	DEBUG_VALID(pcImpl);

	// return pcImpl->GetBaseView()->OnMouseWheel(cInEvent);

	BaseView * pcView = pcImpl->GetBaseView();

	float zDelta = static_cast<float>(cInEvent.GetMouseWheelDelta() / pcView->GetMouseWheelSensitivity() / 120.0 / 8.0);

	HC_Open_Segment_By_Key(pcView->GetSceneKey());

	/* Save the original camera for smooth transition. */
	HCamera cOriginCamera;
	HC_Show_Net_Camera(&cOriginCamera.position, &cOriginCamera.target, &cOriginCamera.up_vector, &cOriginCamera.field_width, &cOriginCamera.field_height, cOriginCamera.projection);

	/* We need to fill in these values with the new camera. */
	HCamera cAdjustedCamera = cOriginCamera;

	if (cInEvent.Shift()) {
		if (zDelta >= 0) {
			cAdjustedCamera.field_width *= 1.2f;
			cAdjustedCamera.field_height *= 1.2f;
		}
		else {
			cAdjustedCamera.field_width *= 0.8f;
			cAdjustedCamera.field_height *= 0.8f;
		}
	}
	else {
		pcImpl->ComputeReasonableTarget(cAdjustedCamera.target, cInEvent.GetMouseWindowPos(), cOriginCamera.target);
		if (streq(cOriginCamera.projection, "perspective")) {
			pcView->ComputeNewField(cAdjustedCamera.field_width, cAdjustedCamera.field_height, cAdjustedCamera.target, cOriginCamera);
		}

		cAdjustedCamera.field_width *= fabs(1 + zDelta);
		cAdjustedCamera.field_height *= fabs(1 + zDelta);

		double dPositionScale = HC_Compute_Vector_Length(&cOriginCamera.position);
		double dTargetScale = HC_Compute_Vector_Length(&cOriginCamera.target);
		double dCameraScale = MAX(dPositionScale, dTargetScale);

		double dDiagonalLength = sqrt(pow(cAdjustedCamera.field_width, 2) + pow(cAdjustedCamera.field_height, 2));

		/* If the camera is about to be too small... */
		if (dDiagonalLength < pcView->GetZoomLimit() || dDiagonalLength / dCameraScale < 1.0e-6) {
			if (zDelta < 0) {
				goto BAILOUT;
			}
			cAdjustedCamera.field_width = cOriginCamera.field_width * fabs(1 + zDelta);
			cAdjustedCamera.field_height = cOriginCamera.field_height * fabs(1 + zDelta);
			dDiagonalLength = sqrt(pow(cAdjustedCamera.field_width, 2) + pow(cAdjustedCamera.field_height, 2));
			cAdjustedCamera.target = cOriginCamera.target;
		}
		/* If the camera is about to be too big... */
		if (dDiagonalLength > fabs(MVO_SQRT_MAX_FLOAT))
			goto BAILOUT;

		/* Shift the target slightly toward the mouse pointer. */
		HVector cMouseWorldPos = cInEvent.GetMouseWorldPos();
		HUtility::AdjustPositionToPlane(pcView, cMouseWorldPos, cAdjustedCamera.target);
		cAdjustedCamera.target += (cAdjustedCamera.target - cMouseWorldPos) * zDelta;

		HVector cDirectionToPosition = cOriginCamera.position - cOriginCamera.target;
		double dOldDiagonalLength = sqrt(pow(cOriginCamera.field_width, 2) + pow(cOriginCamera.field_height, 2));
		double dOldRatio = HC_Compute_Vector_Length(&cDirectionToPosition) / dOldDiagonalLength;
		HC_Compute_Normalized_Vector(&cDirectionToPosition, &cDirectionToPosition);
		cAdjustedCamera.position = cAdjustedCamera.target + cDirectionToPosition * static_cast<float>(dOldRatio * dDiagonalLength);
	}

	if (valid_point(cAdjustedCamera.position) && valid_point(cAdjustedCamera.target) && valid_point(cAdjustedCamera.up_vector) &&
		valid_float(cAdjustedCamera.field_width) && valid_float(cAdjustedCamera.field_height)) {
		if (!cAdjustedCamera.position.Equal(cAdjustedCamera.target, pcView->GetZoomLimit())) {
			pcView->PrepareForCameraChange();
			{
				HC_Set_Camera_Position(cAdjustedCamera.position.x, cAdjustedCamera.position.y, cAdjustedCamera.position.z);
				HC_Set_Camera_Target(cAdjustedCamera.target.x, cAdjustedCamera.target.y, cAdjustedCamera.target.z);
				HC_Set_Camera_Field(cAdjustedCamera.field_width, cAdjustedCamera.field_height);
			}
			pcView->CameraPositionChanged(true, false);

			if (pcView->GetModel()->GetContainsDouble()) {
				HC_Convert_Precision(pcView->GetSceneKey(), "double, camera");
			}

			pcView->Update();
		}
	}

BAILOUT:

	HC_Close_Segment();

	return HLISTENER_CONSUME_EVENT;
}