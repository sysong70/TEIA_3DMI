#include "StdAfx.h"

#include "Operator.CameraSelect.h"
#include "Private/SelectionPrivate.h"

#include "Window.h"
#include "Line.h"

#include "Camera.h"

#include "../Private/View.Private.h"

#include <Common_Define.h>

#include <HBaseView.h>
#include <HTools.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>
#include <HOpCameraPan.h>
#include <HBhvBehaviorManager.h>

CameraZoomBox::CameraZoomBox(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraZoomBox(view, DoRepeat, DoCapture)
{

}

#define MINIMUM_FIELD_SIZE (1e-9f)

// diagonal_len 계산 오류로 인해서 상속해서 사용함.
int CameraZoomBox::OnLButtonUp(HEventInfo & event)
{
	HOpConstructRectangle::OnLButtonUp(event);

	if(GetView()->GetModel()->GetBhvBehaviorManager()->IsPlaying() &&
		GetView()->GetModel()->GetBhvBehaviorManager()->GetCameraUpdated())
		return HOP_OK;

	if(!m_bRectangleExists)
		return HBaseOperator::OnLButtonUp(event);

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
		HCamera adjusted = orig;

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

	bool anything_selected = ComputeReasonableTarget(adjusted.target, m_ptRectangle[0], m_ptRectangle[1], orig.target);

		if(anything_selected) {
			ComputeNewField(adjusted.field_width, adjusted.field_height, m_ptRectangle[0], m_ptRectangle[1], adjusted.target);

			// #Error 3DF: 아래식을 이용해서 값을 계산하면 diagonal_len이 너무 큰값이 나와서 View가 이상해짐.
			//float diagonal_len = static_cast<float>(sqrt(pow(adjusted.field_width, 2)) + pow(adjusted.field_height, 2));
			double diagonal_len = sqrt(pow(adjusted.field_width, 2) + pow(adjusted.field_height, 2));

			HVector viewingVector = orig.position - orig.target;
			HC_Compute_Normalized_Vector(&viewingVector, &viewingVector);
			adjusted.position = adjusted.target + viewingVector * 2.5 * diagonal_len;

			if(m_enforceMinCameraSize) {
				double const camera_dist = HC_Compute_Vector_Length(&adjusted.target) + 1;
				double const min_camera = 0.0005 * camera_dist;

				/* If the camera is about to be too small... */
				if(diagonal_len < min_camera) {
					double x = min_camera / diagonal_len;
					adjusted.field_width *= static_cast<float>(x);
					adjusted.field_height *= static_cast<float>(x);
					diagonal_len = static_cast<float>(sqrt(pow(adjusted.field_width, 2)) +
						static_cast<float>(pow(adjusted.field_height, 2)));
					HVector dir_to_position = orig.position - orig.target;
					HC_Compute_Normalized_Vector(&dir_to_position, &dir_to_position);
					adjusted.target = adjusted.position - dir_to_position * 2.5 * diagonal_len;
				}
			}

			if(orig.CameraDifferent(adjusted)) {
				GetView()->PrepareForCameraChange();
				if(GetView()->GetSmoothTransition()) {
					HUtility::SmoothTransition(orig.position,
						orig.target,
						orig.up_vector,
						orig.field_width,
						orig.field_height,
						adjusted.position,
						adjusted.target,
						adjusted.up_vector,
						adjusted.field_width,
						adjusted.field_height,
						GetView());
			}
				else {
					HC_Set_Camera_Position(adjusted.position.x, adjusted.position.y, adjusted.position.z);
					HC_Set_Camera_Target(adjusted.target.x, adjusted.target.y, adjusted.target.z);
					HC_Set_Camera_Field(adjusted.field_width, adjusted.field_height);
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

int CameraOrbitTurntable::OnLButtonDownAndMove(HEventInfo & event)
{
	if (!OperatorStarted())
		return HBaseOperator::OnLButtonDownAndMove(event);

	SetNewPoint(event.GetMouseWindowPos());

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

H3DF::Operator::CameraSelect::CameraSelect(WindowKey * pcWindow, NavigationCube & cNaviCube, int DoRepeat, int DoCapture) :
	HBaseOperator(pcWindow->GetBaseView(), DoRepeat, DoCapture),
	m_cCameraOrbit(pcWindow->GetBaseView(), DoRepeat, DoCapture),
	m_cCameraOrbitTurntable(pcWindow->GetBaseView(), DoRepeat, DoCapture),
	m_cCameraPan(pcWindow->GetBaseView(), DoRepeat, DoCapture),
	m_cCameraZoomBox(pcWindow->GetBaseView(), DoRepeat, DoCapture)
{
	m_pcWindow = pcWindow;

	m_pcNaviCube = &cNaviCube;

	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
}

H3DF::Operator::CameraSelect::~CameraSelect()
{
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey()); {
		HC_Flush_Contents(".", "geometry");
	} HC_Close_Segment();
}

const char * H3DF::Operator::CameraSelect::GetName()
{
	return "TDF_Operator_CameraOrbitSelect";
}

HBaseOperator * H3DF::Operator::CameraSelect::Clone()
{
	return new H3DF::Operator::CameraSelect(m_pcWindow, *m_pcNaviCube);
}

void H3DF::Operator::CameraSelect::SetViewControlMode(ViewControl::Mode eMode)
{
	m_eViewControlMode = eMode;
}

//== Mouse Event 처리 ===============================================================================

// 1. Left Button Down 처리
int H3DF::Operator::CameraSelect::OnLButtonDown(HEventInfo & cInEvent)
{
	m_cMouseDownPoint = cInEvent.GetMousePixelPos();
	m_nMouseDownTickCount = GetTickCount();

	// Shift & L Button 이벤트는 Area Select
	if (MVO_SHIFT & cInEvent.GetFlags()) {
	}

	switch (m_eViewControlMode)
	{
		case H3DF::ViewControl::Mode::OrbitTurntable:
			return m_cCameraOrbitTurntable.OnLButtonDown(cInEvent);
			break;

		case H3DF::ViewControl::Mode::Pan:
			return m_cCameraPan.OnLButtonDown(cInEvent);
			break;

		case H3DF::ViewControl::Mode::ZoomBox:
			return m_cCameraZoomBox.OnLButtonDown(cInEvent);
			break;
	}

	return m_cCameraOrbit.OnLButtonDown(cInEvent);
}

// 2. Left Button Up 처리
// L Button Up을 핱때 Objet를 선택함.
int H3DF::Operator::CameraSelect::OnLButtonUp(HEventInfo & cInEvent)
{
	if (nullptr != m_pcNaviCube) {
		if (HLISTENER_CONSUME_EVENT == m_pcNaviCube->LButtonUp(cInEvent)) {
			return HLISTENER_CONSUME_EVENT;
		}
	}

	switch (m_eViewControlMode)
	{
		case H3DF::ViewControl::Mode::OrbitTurntable:
			return m_cCameraOrbitTurntable.OnLButtonUp(cInEvent);
			break;

		case H3DF::ViewControl::Mode::Pan:
			return m_cCameraPan.OnLButtonUp(cInEvent);
			break;

		case H3DF::ViewControl::Mode::ZoomBox:
			return m_cCameraZoomBox.OnLButtonUp(cInEvent);
			break;
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

	m_cClickPoint = cInEvent.GetMouseWorldPos();

	return m_cCameraOrbit.OnLButtonUp(cInEvent);
}

int H3DF::Operator::CameraSelect::OnLButtonDownAndMove(HEventInfo & cInEvent)
{
	m_pcWindow->GetBaseView()->SetSuppressUpdate(true);

	if (nullptr != m_pcNaviCube) {
		m_pcNaviCube->LButtonDownAndMove(cInEvent);
	}

	if (nullptr != m_pcNaviCube) {
		m_pcNaviCube->Transform();
	}

	int nResult = 0;

	switch (m_eViewControlMode)
	{
		case H3DF::ViewControl::Mode::OrbitTurntable:
			nResult = m_cCameraOrbitTurntable.OnLButtonDownAndMove(cInEvent);
			break;
			
		case H3DF::ViewControl::Mode::Pan:
			nResult = m_cCameraPan.OnLButtonDownAndMove(cInEvent);
			break;

		case H3DF::ViewControl::Mode::ZoomBox:
			nResult = m_cCameraZoomBox.OnLButtonDownAndMove(cInEvent);
			break;

		default:
			nResult = m_cCameraOrbit.OnLButtonDownAndMove(cInEvent);
			break;
	}

	m_pcWindow->GetBaseView()->SetSuppressUpdate(false);

	GetView()->Update();

	return nResult;
}

int H3DF::Operator::CameraSelect::OnRButtonDown(HEventInfo & hevent)
{
	return m_cCameraPan.OnLButtonDown(hevent);
}

int H3DF::Operator::CameraSelect::OnRButtonDownAndMove(HEventInfo & hevent)
{
	return m_cCameraPan.OnLButtonDownAndMove(hevent);
}

int H3DF::Operator::CameraSelect::OnRButtonUp(HEventInfo & hevent)
{
	return m_cCameraPan.OnLButtonUp(hevent);
}

// Dynamic Highlighting 처리
int H3DF::Operator::CameraSelect::OnNoButtonDownAndMove(HEventInfo & cInEvent)
{
/*
	if (nullptr != m_pcNaviCube) {
		if (HLISTENER_CONSUME_EVENT == m_pcNaviCube->NoButtonDownAndMove(cInEvent)) {
			return HLISTENER_CONSUME_EVENT;
		}
	}
*/

	return HLISTENER_PASS_EVENT;
}

//== Mouse Event 처리 ===============================================================================
int H3DF::Operator::CameraSelect::OnMouseWheel(HEventInfo & cInEvent)
{
	int nResult = m_pcWindow->GetBaseView()->OnMouseWheel(cInEvent);

	return nResult;
}