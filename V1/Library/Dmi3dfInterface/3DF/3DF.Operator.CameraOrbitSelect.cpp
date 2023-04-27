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

int Operator::CameraOrbitSelect::OnLButtonDown(HEventInfo & cInEvent)
{
	m_cMouseDownPoint = cInEvent.GetMousePixelPos();
	m_nMouseDownTickCount = GetTickCount();
	m_bOrbitMode = false;

	// Shift & L Button 이벤트는 Area Select
	if (MVO_SHIFT & cInEvent.GetFlags()) {
	}

	return HOpCameraOrbit::OnLButtonDown(cInEvent);
}

int Operator::CameraOrbitSelect::OnLButtonUp(HEventInfo & cInEvent)
{
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

			TDF::SelectionResults cResult;
			m_pcWindow->GetSelectionControl().SelectByPoint(cPoint, cInEvent.GetFlags(), cResult);
		}
	}

	m_bOrbitMode = false;

	m_cClickPoint = cInEvent.GetMouseWorldPos();

	return HOpCameraOrbit::OnLButtonUp(cInEvent);
}

int Operator::CameraOrbitSelect::OnLButtonDownAndMove(HEventInfo & cInEvent)
{
	// PMI Test	Code
/*
	SegmentKey cSecne(m_pcWindow->GetSceneKey());

	CameraKit cCamera;
	cSecne.ShowCamera(cCamera);

	Matrix cMatrix;
	cCamera.ShowMatrix(cMatrix);

	WorldPoint cPoint[2];

	cPoint[0] = m_cClickPoint;
	cPoint[1] = cInEvent.GetMouseWorldPos();

	// Test Object Snap
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey()); {
		HDraw::Test(GetView(), cMatrix, cPoint[0], cPoint[1]);
	} HC_Close_Segment();
*/

	m_bOrbitMode = true;
	return HOpCameraOrbit::OnLButtonDownAndMove(cInEvent);
}

// Dynamic Highlighting 처리
int Operator::CameraOrbitSelect::OnNoButtonDownAndMove(HEventInfo & cInEvent)
{
	SelectionOptionsKit cSelectOption;
	cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(10).SetProximity(0.2f).SetSorting(Selection::Sorting::ZSorting);

	SelectionResults cHighlightSelection;
	size_t nSelectedCount = m_pcWindow->GetSelectionControl().SelectByPoint(cInEvent, cSelectOption, cHighlightSelection);

	// Selection된 Item들에서 Windows Point의 Z값을 이용해서 Sort하도록 한다.
	// 1.나오는 Item은 이미 Sorting이 되어 있음.
	// 2.맨앞에 나온 요소가 ShellKey이고, 같은 Z값에 LineKey가 있는 경우 LineKey를 사용하도록 한다.

	// 신규 선택 요소 저장소는 초기화한다.
	m_cNewHighlightSelection.Reset();

	if (0 < nSelectedCount) {
		SelectionResultsIterator cIter = cHighlightSelection.GetIterator();

		if (true == cIter.IsValid()) {
			SelectionItem * pcItem = cIter.GetItem();

			Key cSelectKey;
			pcItem->ShowSelectedItem(cSelectKey);

			// 나오는 요소의 종류를 확인한다.
			Type eType = cSelectKey.Type();

			m_cNewHighlightSelection.PushBack(new SelectionItem(*pcItem));

			if (Type::LineKey == eType) {
				m_cNewHighlightSelection.PushBack(new SelectionItem(*pcItem));
			}
		}
	}

/*
	bool bFindShellKey = false; // ShellKey를 찾았는지 여부
	float fZValue = 0.0f; // Z값을 저장;

	while (true == cIter.IsValid()) {
		SelectionItem * pcItem = cIter.GetItem();

		// Window Point를 얻어서 Z값을 얻는다.
		WindowPoint cWindowPoint;
		pcItem->ShowSelectionPosition(cWindowPoint);

		Key cSelectKey;
		pcItem->ShowSelectedItem(cSelectKey);

		// 나오는 요소의 종류를 확인한다.
		Type eType = cSelectKey.Type();

		if (Type::LineKey == eType) {
			m_cNewHighlightSelection.PushBack(new SelectionItem(*pcItem));
			TRACE(L"LineKey, \t%f\n", cWindowPoint.z);
			break;
		}
		// Sort가 잘되어서 별도로 처리하지 않아도 될것 같음.
		// 일단 Code는 남겨둠.
		else if (Type::ShellKey == eType) {
			//m_cNewHighlightSelection.PushBack(new SelectionItem(*pcItem));
			bFindShellKey = true;
			fZValue = cWindowPoint.z;

			TRACE(L"ShellKey, \t%f\n", cWindowPoint.z);
		}

		cIter.Next();
	}
*/

	// Old와 New가 다르면 Old를 Unhiglight하고 Reset 시킨다.
	if (0 < m_cOldHighlightSelection.GetCount() && m_cOldHighlightSelection != m_cNewHighlightSelection) {
		m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection);
		m_cOldHighlightSelection.Reset();
	}

	// 	새롭게 선택된 Selection Result에서 Line만 남기도록 한다.
	// 	m_cNewHighlightSelection.LeaveType((DWORD)TDF::Type::LineKey);

	// 추가된것이 있는 경우에 Count를 검사해서 5개까지만 남기도록 한다.
	if (true == m_cHighlightSelection.Union(m_cNewHighlightSelection)) {
		if (5 < m_cHighlightSelection.GetCount()) {
			m_cHighlightSelection.SetSize(5);
		}
	}

	m_cOldHighlightSelection = m_cNewHighlightSelection;

	HighlightOptionsKit cKit;

	if (0 < m_cNewHighlightSelection.GetCount()) {
		m_pcWindow->GetHighlightControl().Highlight(m_cNewHighlightSelection, cKit);

		Operator::ObjectSnap cSnap(m_pcWindow);
		cSnap.DrawObjectSnapPoint(m_cHighlightSelection);
	}
	else {
		// Object Snape 등을 지우도록 한다.
		HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetConstructionKey()); {
			HC_Flush_Contents(".", "geometry, segment");
		} HC_Close_Segment();

		m_pcWindow->GetBaseView()->Update();
	}

	return HLISTENER_PASS_EVENT;

	// PMI Test	Code

	SegmentKey cSecne(m_pcWindow->GetSceneKey());

	CameraKit cCamera;
	cSecne.ShowCamera(cCamera);

	Matrix cMatrix;
	cCamera.ShowMatrix(cMatrix);

	WorldPoint cPoint[2];
	
	cPoint[0] = m_cClickPoint;
	cPoint[1] = cInEvent.GetMouseWorldPos();

	Vector cXAixs = cMatrix.XAxis();
	Vector cYAixs = cMatrix.YAxis();
	Vector cOrigin = cMatrix.Origin();

	Point2D cP1 = cPoint[0].DropPoint(cOrigin, cXAixs, cYAixs);
	Point2D cP2 = cPoint[1].DropPoint(cOrigin, cXAixs, cYAixs);

	// Test Object Snap
	HC_Open_Segment_By_Key(GetView()->GetConstructionKey()); {
		HDraw::Test(GetView(), cMatrix, cP1, cP2);
	} HC_Close_Segment();

	GetView()->Update();

	return HLISTENER_PASS_EVENT;
}

