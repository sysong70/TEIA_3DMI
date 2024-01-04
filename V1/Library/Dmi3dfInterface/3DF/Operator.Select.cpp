#include "StdAfx.h"

#include "Operator.Select.h"
#include "Impl/SelectionImpl.h"

#include "Window.h"
#include "Line.h"

#include "Camera.h"

#include "Selection.h"
#include "Highlight.h"

#include "../Impl/ViewImpl.h"

#include <Common_Define.h>

#include <HBaseView.h>
#include <HTools.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>
#include <HOpCameraPan.h>
#include <HBhvBehaviorManager.h>

using namespace H3DF;

H3DF::Operator::Select::Select(WindowKey * pcWindow, NavigationCube & cNaviCube, int DoRepeat, int DoCapture) :
	HBaseOperator(pcWindow->GetBaseView(), DoRepeat, DoCapture)
{
	m_pcWindow = pcWindow;

	m_pcNaviCube = &cNaviCube;

	m_nSelectTickCount = 200;
	m_nMouseDownTickCount = 0;
}

H3DF::Operator::Select::~Select()
{
}

const char * H3DF::Operator::Select::GetName()
{
	return "H3DF_Operator_CameraOrbitSelect";
}

HBaseOperator * H3DF::Operator::Select::Clone()
{
	return new H3DF::Operator::Select(m_pcWindow, *m_pcNaviCube);
}

//== Mouse Event 처리 ===============================================================================

// 1. Left Button Down 처리
int H3DF::Operator::Select::OnLButtonDown(HEventInfo & cInEvent)
{
	m_cMouseDownPoint = cInEvent.GetMousePixelPos();
	m_nMouseDownTickCount = GetTickCount();

	// Shift & L Button 이벤트는 Area Select
	if (MVO_SHIFT & cInEvent.GetFlags()) {
	}

	return HLISTENER_PASS_EVENT;
}

// 2. Left Button Up 처리
// L Button Up을 핱때 Objet를 선택함.
int H3DF::Operator::Select::OnLButtonUp(HEventInfo & cInEvent)
{
	if (nullptr != m_pcNaviCube) {
		if (HLISTENER_CONSUME_EVENT == m_pcNaviCube->LButtonUp(cInEvent)) {
			return HLISTENER_CONSUME_EVENT;
		}
	}

	DWORD nMouseUpTickCount = GetTickCount();
	DWORD nTickCount = nMouseUpTickCount - m_nMouseDownTickCount;
	size_t nSelectCount = 0;

	// 2 Pixel이하 200 Tick이하에서만 선택하는 것으로 판정한다.
	if (m_nSelectTickCount > nTickCount) {
		const HPoint & cMoustPoint = cInEvent.GetMousePixelPos();
		HVector cVector = cMoustPoint - m_cMouseDownPoint;
		double dLength = HC_Compute_Vector_Length(&cVector);

		if (2.0 > dLength) {
			Point cPoint;
			cPoint.x = cInEvent.GetMouseWindowPos().x;
			cPoint.y = cInEvent.GetMouseWindowPos().y;

			H3DF::SelectionResults cResult;
			nSelectCount = m_pcWindow->GetSelectionControl().SelectByPoint(cPoint, cInEvent.GetFlags(), cResult);

			HighlightOptionsKit cHighlightOptions;
			m_pcWindow->GetHighlightControl().Highlight(cResult, cHighlightOptions, false);//.Highlight(cResult);
		}
	}

	m_cClickPoint = cInEvent.GetMouseWorldPos();

	if (0 < nSelectCount) {
		return HLISTENER_CONSUME_EVENT;
	}

	return HLISTENER_PASS_EVENT;
}

int H3DF::Operator::Select::OnLButtonDownAndMove(HEventInfo & cInEvent)
{
	return HLISTENER_PASS_EVENT;
}

int H3DF::Operator::Select::OnNoButtonDownAndMove(HEventInfo & cInEvent)
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