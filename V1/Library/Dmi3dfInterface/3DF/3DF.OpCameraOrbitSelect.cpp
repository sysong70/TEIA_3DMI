#include "StdAfx.h"

#include "3DF.OpCameraOrbitSelect.h"

#include "3DF.Window.h"

#include <Common_Define.h>

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>

USING_3DF_NAMESPACE

OpCameraOrbitSelect::OpCameraOrbitSelect(WindowKey * pcWindow, int DoRepeat, int DoCapture) :
	HOpCameraOrbit(pcWindow->GetBaseView(), DoRepeat, DoCapture)
{
	m_pcWindow = pcWindow;

	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
	m_bOrbitMode = false;

}

OpCameraOrbitSelect::OpCameraOrbitSelect(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraOrbit(view, DoRepeat, DoCapture)
{
	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
	m_bOrbitMode = false;

}

OpCameraOrbitSelect::~OpCameraOrbitSelect()
{
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey());
	HC_Flush_Contents(".", "geometry");
	HC_Close_Segment();
}

const char * OpCameraOrbitSelect::GetName()
{
	return "3DF_OpCameraOrbitSelect";
}

HBaseOperator * OpCameraOrbitSelect::Clone()
{
	return new OpCameraOrbitSelect(GetView());
}
//== Mouse Event 처리 ===============================================================================

int OpCameraOrbitSelect::OnLButtonDown(HEventInfo & cEvent)
{
	m_cMouseDownPoint = cEvent.GetMousePixelPos();
	m_nMouseDownTickCount = GetTickCount();
	m_bOrbitMode = false;

	// Shift & L Button 이벤트는 Area Select
	if (MVO_SHIFT & cEvent.GetFlags()) {
	}

	return HOpCameraOrbit::OnLButtonDown(cEvent);
}

int OpCameraOrbitSelect::OnLButtonUp(HEventInfo & cEvent)
{
	DWORD nMouseUpTickCount = GetTickCount();
	DWORD nTickCount = nMouseUpTickCount - m_nMouseDownTickCount;

	// 2 Pixel이하 200 Tick이하에서만 선택하는 것으로 판정한다.
	if (m_nSelectPickCount > nTickCount) {
		const HPoint & cMoustPoint = cEvent.GetMousePixelPos();
		HVector cVector = cMoustPoint - m_cMouseDownPoint;
		double dLength = HC_Compute_Vector_Length(&cVector);

		if (2.0 > dLength) {
			Point cPoint;
			cPoint.x = cEvent.GetMouseWindowPos().x;
			cPoint.y = cEvent.GetMouseWindowPos().y;

			_3DF::SelectionResults cResult;
			m_pcWindow->GetSelectionControl().SelectByPoint(cPoint, cEvent.GetFlags(), cResult);
		}
	}

	m_bOrbitMode = false;

	return HOpCameraOrbit::OnLButtonUp(cEvent);
}

int OpCameraOrbitSelect::OnLButtonDownAndMove(HEventInfo & cEvent)
{
	m_bOrbitMode = true;
	return HOpCameraOrbit::OnLButtonDownAndMove(cEvent);
}

int OpCameraOrbitSelect::OnNoButtonDownAndMove(HEventInfo & cEvent)
{
	return OnDaynamicHighlightMouseMove(cEvent);

/*
	HPoint  new_pos;
	new_pos = event.GetMouseWindowPos();

	DoDynamicHighlighting(new_pos);
	return HLISTENER_PASS_EVENT;
*/
}

//== Selection 관련 함수 =============================================================================

// Mouse를 이동할 때 Highlight 처리
int  OpCameraOrbitSelect::OnDaynamicHighlightMouseMove(HEventInfo & cEvent)
{
	if (nullptr == m_pcWindow) {
		return HLISTENER_PASS_EVENT;
	}

	size_t nSelectedCount = m_pcWindow->GetSelectionControl().SelectByPoint(cEvent, m_cNewHighlightSelection);

	if (0 < m_cNewHighlightSelection.GetCount()) {
		m_cNewHighlightSelection.Reset();
	}

	if (0 < m_cOldHighlightSelection.GetCount()) {
		//m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection);
	}

	return HLISTENER_PASS_EVENT;
}

