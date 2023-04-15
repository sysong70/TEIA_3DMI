#include "StdAfx.h"

#include "3DF.Operator.CameraOrbitSelect.h"
#include "Private/3DF.SelectionPrivate.h"

#include "3DF.Window.h"
#include "3DF.Line.h"

#include "3DF.Camera.h"

#include "3DF.Operator.ObjectSnap.h"

#include "HDraw.h"

#include <Common_Define.h>

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>

USING_3DF_NAMESPACE

Operator::CameraOrbitSelect::CameraOrbitSelect(WindowKey * pcWindow, int DoRepeat, int DoCapture) :
	HOpCameraOrbit(pcWindow->GetBaseView(), DoRepeat, DoCapture)
{
	m_pcWindow = pcWindow;

	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
	m_bOrbitMode = false;

}

Operator::CameraOrbitSelect::CameraOrbitSelect(HBaseView * view, int DoRepeat, int DoCapture) :
	HOpCameraOrbit(view, DoRepeat, DoCapture)
{
	m_nSelectPickCount = 200;
	m_nMouseDownTickCount = 0;
	m_bOrbitMode = false;

}

Operator::CameraOrbitSelect::~CameraOrbitSelect()
{
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey());
	HC_Flush_Contents(".", "geometry");
	HC_Close_Segment();
}

const char * Operator::CameraOrbitSelect::GetName()
{
	return "3DF_Operator::CameraOrbitSelect";
}

HBaseOperator * Operator::CameraOrbitSelect::Clone()
{
	return new Operator::CameraOrbitSelect(GetView());
}
//== Mouse Event 처리 ===============================================================================

int Operator::CameraOrbitSelect::OnLButtonDown(HEventInfo & cEvent)
{
	m_cMouseDownPoint = cEvent.GetMousePixelPos();
	m_nMouseDownTickCount = GetTickCount();
	m_bOrbitMode = false;

	// Shift & L Button 이벤트는 Area Select
	if (MVO_SHIFT & cEvent.GetFlags()) {
	}

	return HOpCameraOrbit::OnLButtonDown(cEvent);
}

int Operator::CameraOrbitSelect::OnLButtonUp(HEventInfo & cEvent)
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

			TDF::SelectionResults cResult;
			m_pcWindow->GetSelectionControl().SelectByPoint(cPoint, cEvent.GetFlags(), cResult);
		}
	}

	m_bOrbitMode = false;

	m_cClickPoint = cEvent.GetMouseWorldPos();

	return HOpCameraOrbit::OnLButtonUp(cEvent);
}

int Operator::CameraOrbitSelect::OnLButtonDownAndMove(HEventInfo & cEvent)
{
	m_bOrbitMode = true;
	return HOpCameraOrbit::OnLButtonDownAndMove(cEvent);
}

// Dynamic Highlighting 처리
int Operator::CameraOrbitSelect::OnNoButtonDownAndMove(HEventInfo & cEvent)
{
	SelectionOptionsKit cSelectOption;
	cSelectOption.SetLevel(Selection::Level::Entity);
	cSelectOption.SetRelatedLimit(0);
	cSelectOption.SetInternalLimit(0);
	cSelectOption.SetSorting(Selection::Sorting::Default);

	m_cNewHighlightSelection.Reset();
	size_t nSelectedCount = m_pcWindow->GetSelectionControl().SelectByPoint(cEvent, cSelectOption, m_cNewHighlightSelection);

	// Old와 New가 다르면 Old를 Unhiglight하고 Reset 시킨다.
	if (0 < m_cOldHighlightSelection.GetCount() && m_cOldHighlightSelection != m_cNewHighlightSelection) {
		m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection);
		m_cOldHighlightSelection.Reset();
	}

	// 새롭게 선택된 Selection Result에서 Line만 남기도록 한다.
	m_cNewHighlightSelection.LeaveType((DWORD)TDF::Type::LineKey);

	// 추가된것이 있는 경우에 Count를 검사해서 5개까지만 남기도록 한다.
	if (true == m_cHighlightSelection.Union(m_cNewHighlightSelection)) {
		if (5 < m_cHighlightSelection.GetCount()) {
			m_cHighlightSelection.SetSize(5);
		}
	}

	TRACE(L"HighlightSelection Count: %d\n", m_cHighlightSelection.GetCount());

	m_cOldHighlightSelection = m_cNewHighlightSelection;


	HighlightOptionsKit cKit;

	if (0 < m_cNewHighlightSelection.GetCount()) {
		m_pcWindow->GetHighlightControl().Highlight(m_cNewHighlightSelection, cKit);
// 	if (0 < m_cHighlightSelection.GetCount()) {
// 			m_pcWindow->GetHighlightControl().Highlight(m_cHighlightSelection, cKit);

		Operator::ObjectSnap cSnap(m_pcWindow);
		cSnap.DrawObjectSnapPoint(m_cHighlightSelection);
	}
	else {
		// Object Snape 등을 지우도록 한다.
		// 아래 code 테스트용으로 임시 Remark
/*
		HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetConstructionKey()); {
			HC_Flush_Contents(".", "geometry, segment");
		} HC_Close_Segment();

		m_pcWindow->GetBaseView()->Update();*/
	}

	SegmentKey cSecne(m_pcWindow->GetSceneKey());

	CameraKit cCamera;
	cSecne.ShowCamera(cCamera);

	Matrix cMatrix;
	cCamera.ShowMatrix(cMatrix);

	Point cPoint[2];
	cPoint[0].x = m_cClickPoint.x;
	cPoint[0].y = m_cClickPoint.y;
	cPoint[0].z = m_cClickPoint.z;

	cPoint[1].x = cEvent.GetMouseWorldPos().x;
	cPoint[1].y = cEvent.GetMouseWorldPos().y;
	cPoint[1].z = cEvent.GetMouseWorldPos().z;


	// Test Object Snap
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey()); {
		HDraw::Test(GetView(), cMatrix, cPoint[0], cPoint[1]);
	} HC_Close_Segment();

	GetView()->Update();

	return HLISTENER_PASS_EVENT;
}
