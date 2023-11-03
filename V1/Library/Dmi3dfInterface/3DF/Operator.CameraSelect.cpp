#include "StdAfx.h"

#include "Operator.CameraSelect.h"
#include "Private/SelectionPrivate.h"

#include "Window.h"
#include "Line.h"

#include "Camera.h"

#include "Operator.ObjectSnap.h"

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

USING_3DF_NAMESPACE

H3DF::Operator::CameraSelect::CameraSelect(WindowKey * pcWindow, NavigationCube & cNaviCube, int DoRepeat, int DoCapture) :
	HBaseOperator(pcWindow->GetBaseView(), DoRepeat, DoCapture),
	m_cCameraOrbit(pcWindow->GetBaseView(), DoRepeat, DoCapture),
	m_cCameraPan(pcWindow->GetBaseView(), DoRepeat, DoCapture),
	m_cCameraZoomBox(pcWindow->GetBaseView(), DoRepeat, DoCapture),
	m_cObjectSnapOperator(pcWindow)
{
	m_pcWindow = pcWindow;

	m_pcNaviCube = &cNaviCube;

	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
}

/*
H3DF::Operator::CameraSelect::CameraSelect(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraOrbit(view, DoRepeat, DoCapture)
{
	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
}
*/

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
		case H3DF::ViewControl::Mode::Pan:
			return m_cCameraPan.OnLButtonDown(cInEvent);
			break;
		case H3DF::ViewControl::Mode::ZoomBox:
			m_cCameraZoomBox.SetLightFollowsCamera(true);
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
		case H3DF::ViewControl::Mode::Pan:
			return m_cCameraPan.OnLButtonUp(cInEvent);
			break;

		case H3DF::ViewControl::Mode::ZoomBox:
			return OnZoomBoxLButtonUp(cInEvent);
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

// 2.1 Zoom Box L Button Up 처리
int H3DF::Operator::CameraSelect::OnZoomBoxLButtonUp(HEventInfo & cInEvent)
{
	m_pcWindow->GetBaseView()->SetSuppressUpdate(true);

	int nResult = m_cCameraZoomBox.OnLButtonUp(cInEvent);

	m_cObjectSnapOperator.DrawSnapItems();

	m_pcWindow->GetBaseView()->SetSuppressUpdate(false);

	m_pcWindow->GetBaseView()->Update();

	return nResult;
}

int H3DF::Operator::CameraSelect::OnLButtonDownAndMove(HEventInfo & cInEvent)
{
	if (nullptr != m_pcNaviCube) {
		m_pcNaviCube->LButtonDownAndMove(cInEvent);
	}

	m_pcWindow->GetBaseView()->SetSuppressUpdate(true);

	m_cObjectSnapOperator.DrawSnapItems();

// 	if (nullptr != m_pcNaviCube) {
// 		m_pcNaviCube->Transform();
// 	}

	int nResult = 0;

	switch (m_eViewControlMode)
	{
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
	if (nullptr != m_pcNaviCube) {
		if (HLISTENER_CONSUME_EVENT == m_pcNaviCube->NoButtonDownAndMove(cInEvent)) {
			return HLISTENER_CONSUME_EVENT;
		}
	}

	m_cObjectSnapOperator.NoButtonDownAndMove(cInEvent);

	return HLISTENER_PASS_EVENT;
}

//== Mouse Event 처리 ===============================================================================
int H3DF::Operator::CameraSelect::OnMouseWheel(HEventInfo & cInEvent)
{
	m_pcWindow->GetBaseView()->SetSuppressUpdate(true);

	int nResult = m_pcWindow->GetBaseView()->OnMouseWheel(cInEvent);

	m_cObjectSnapOperator.DrawSnapItems();

	m_pcWindow->GetBaseView()->SetSuppressUpdate(false);

	m_pcWindow->GetBaseView()->Update();

	return nResult;
}