#include "StdAfx.h"

#include "OPERATOR.HighlightObjectSnapPrivate.h"

#include <Private/View.Private.h>

#include <3DF/Window.h>
#include <3DF/Private/WindowPrivate.h>
#include <3DF/Private/SegmentPrivate.h>

#include <3DF/Line.h>
#include <3DF/Circle.h>
#include <3DF/Point.h>

#include <3DF/Math.Matrix.h>

#include <3DF/Camera.h>
#include <3DF/Color.h>
#include <3DF/Material.h>

#include <3DF/Selection.h>

#include <3DF/Highlight.h>
#include <3DF/Visibility.h>
#include <3DF/VisualEffects.h>

#include <Signal.Connector.h>

#include <Common_Define.h>

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>

#include <3DF/Painter.h>

#include <unordered_set>

#include <3DF/Facility.AppOptions.h>
//#include <3DF/Facility.Preference.h>

H3DF::Facility::AppOptions TheAppOptions;

// #define OBJECT_SNAP_PRIVATE_TRACE

using namespace KERNEL;
using namespace H3DF;

#define TheEnvironment TheAppOptions.Preference.Environment
#define TheSession TheAppOptions.Preference.Session

//== SnapPoint class ===============================================================================

KERNEL::Operator::HighlightObjectSnapPrivate::SnapPoint::SnapPoint(KERNEL::Operator::HighlightObjectSnapPrivate::SnapPoint const & cInThat)
{
	cPoint = cInThat.cPoint;
	eType = cInThat.eType;
	eStatus = cInThat.eStatus;
}

KERNEL::Operator::HighlightObjectSnapPrivate::SnapPoint & KERNEL::Operator::HighlightObjectSnapPrivate::SnapPoint::operator = (KERNEL::Operator::HighlightObjectSnapPrivate::SnapPoint const & cInThat)
{
	cPoint = cInThat.cPoint;
	eType = cInThat.eType;
	eStatus = cInThat.eStatus;

	return *this;
}

//== SnapItem class ================================================================================
// 
// Select Item의 구성 요소가 같은지 확인한다.
bool KERNEL::Operator::HighlightObjectSnapPrivate::SnapItem::operator == (const SnapItem & cInThat) const
{
/*
	if (vcSnapPoints.size() != cInThat.vcSnapPoints.size()) {
		return false;
	}

	// Point의 값이 같은지 확인한다.
	for (size_t i = 0; i < vcSnapPoints.size(); ++i) {
		if (vcSnapPoints[i].cPoint != cInThat.vcSnapPoints[i].cPoint) {
			return false;
		}
	}
*/
	if (vcItems.size() != cInThat.vcItems.size()) {
		return false;
	}

	for (auto & cSelItem : vcItems) {
		bool bSameFind = false;
		for (auto & cInSelItem : cInThat.vcItems) {
			if (cSelItem == cInSelItem) {
				bSameFind = true;
				break;
			}
		}

		if(false == bSameFind) {
			return false;
		}
	}

	return true;
}


//== ObjectSnap class ==============================================================================
KERNEL::Operator::HighlightObjectSnapPrivate::HighlightObjectSnapPrivate(H3DF::WindowKey * pcWindow)
{
	m_pcWindow = pcWindow;

	SegmentKey cConstruction(m_pcWindow->GetBaseView()->GetConstructionKey());
	//SegmentKey cConstruction(m_pcWindow->GetBaseView()->GetSceneKey());

	m_cSnapPointSegment = cConstruction.Subsegment(L"SnapPoint");

	m_cSnapPointSegment.Open();
		HC_Set_Heuristics("quick moves, no backplane culling, no hidden surfaces");
		HC_Set_Selectability("everything = off");
		HC_Set_Visibility("lights = off, cutting planes = off, text = on, markers = off");
		HC_Set_Visibility("no shadows");
 		HC_Set_Rendering_Options("no display lists");
 		HC_Set_Rendering_Options("no frame buffer effects");
 		HC_Set_Heuristics("exclude bounding");
	m_cSnapPointSegment.Close();

	// Snap Point Segment 설정
	//m_cSnapPointSegment = cConstruction.Subsegment(L"SnapPoint");

	MaterialMappingKit cMaterialMapping;
	cMaterialMapping.SetEdgeColor(RGBAColor(0, 0, 0));
	cMaterialMapping.SetFaceColor(RGBAColor(1, 1, 1));

	m_cSnapPointSegment.SetMaterialMapping(cMaterialMapping);
	m_cSnapPointSegment.GetVisibilityControl().SetFaces(true);
	m_cSnapPointSegment.GetVisibilityControl().SetLines(true);
	m_cSnapPointSegment.GetVisibilityControl().SetEdges(true);
	m_cSnapPointSegment.GetEdgeAttributeControl().SetWeight(3.0, Edge::SizeUnits::Pixels);
	//m_cSnapPointSegment.GetEdgeAttributeControl().SetWeight(5.0);
	//m_cSnapPointSegment.GetVisualEffectsControl().SetAntiAliasing(true);
	m_cSnapPointSegment.GetVisualEffectsControl().SetLineAntiAliasing(true);
	m_cSnapPointSegment.GetVisualEffectsControl().SetTextAntiAliasing(true);

	// Tick Count 초기화
	m_nPrevMouseMoveTickCount = GetTickCount();
	m_nSelectPickCount = 300;

	m_nOSnapMode += (DWORD) OSnap::Type::EndPoint;
	m_nOSnapMode += (DWORD) OSnap::Type::MidPoint;
	m_nOSnapMode += (DWORD) OSnap::Type::Center;
	m_nOSnapMode += (DWORD) OSnap::Type::Intersection;
	m_nOSnapMode += (DWORD) OSnap::Type::Perpendicular;
	m_nOSnapMode += (DWORD) OSnap::Type::Quadrant;
	m_nOSnapMode += (DWORD) OSnap::Type::OnSurface;
	m_nOSnapMode += (DWORD) OSnap::Type::Axis;
}

//== Mouse Event ===================================================================================

// 1. Left 버튼 눌림 있는 Mouse Move 처리
int KERNEL::Operator::HighlightObjectSnapPrivate::LButtonDownAndMove(int nFlags, int x, int y)
{
	DrawSnapItems();

	return HLISTENER_PASS_EVENT;
}

// 2. 버튼 눌림 없는 Mouse Move 처리
int KERNEL::Operator::HighlightObjectSnapPrivate::NoButtonDownAndMove(int nFlags, int x, int y)
{
	PixelPoint cMousePoint(x, y);

	DWORD nMouseMoveTickCount = GetTickCount();
	DWORD nTickCount = nMouseMoveTickCount - m_nPrevMouseMoveTickCount;
	m_nPrevMouseMoveTickCount = nMouseMoveTickCount;

	float fDist = m_cPrevPoint.DistanceWith(cMousePoint);
	m_cPrevPoint = cMousePoint;

#ifdef OBJECT_SNAP_PRIVATE_TRACE
	TRACE(L"ObjectSnapPrivate::NoButtonDownAndMove, Tick: %d, Dist: %f\n", nTickCount, fDist);
#endif

	// 같은 Mouse Point가 계속 들어오는 경우는 처리하지 않는다.
	if (3 < fDist || 0 == fDist) {
		return HLISTENER_PASS_EVENT;
	}

	WindowPoint cWindowPoint(*m_pcWindow, cMousePoint);

	SelectionResults cSelections;
	DoDynamicHighlighting(cWindowPoint, cSelections);

	//TRACE(L"ObjectSnapPrivate::NoButtonDownAndMove, Dist: %f\n", fDist);

	CamerInformation cCameraInfo;
	ShowCameraInformation(m_fSnapRadius, cCameraInfo);

	// 기존에 선택된 Snap Point가 있으면 삭제한다. Segment를 Flush한다.
	m_cSnapPointSegment.Flush(Search::Type::Segment);

	// 저장되어 있는 Snap Point를 그림.
	for (auto & pcSnapItem : m_vSnapItems) {
		for (auto & cSnapPoint : pcSnapItem->vcSnapPoints) {
			cSnapPoint.eStatus = HighlightObjectSnapPrivate::Status::Normal;

			if (OSnap::Type::NearPoint == cSnapPoint.eType) {
				continue;
			}

			DrawSnapPoint(cSnapPoint, cCameraInfo);
		}
	}

	// 저장되어 있는 Snap Point를 선택해서 처리하는 부분
	for (auto & pcSnapItem : m_vSnapItems) {
		for (auto & cSnapPoint : pcSnapItem->vcSnapPoints) {
			if (OSnap::Type::NearPoint == cSnapPoint.eType) {
				continue;
			}

			PixelPoint cPixelPoint(*m_pcWindow, cSnapPoint.cPoint);

			double dDist = cPixelPoint.DistanceWith(cMousePoint);

			if (15 > dDist) {
				cSnapPoint.eStatus = HighlightObjectSnapPrivate::Status::Selected;

				// 기존에 선택된 Snap Point가 있으면 삭제한다.
				// m_cSnapPointSegment.Flush(Search::Type::Segment);

				//TRACE(L"1st DrawSnapItem, %d\n", (int)pcSnapItem->eType);
				DrawSnapPoint(cSnapPoint, cCameraInfo);

				HighlightOptionsKit cHighlightOptions;
				cHighlightOptions.SetNotification(false);

				// 맨 처음에는 기존 Hightlight를 삭제한다.
				bool bInRemoveExisting = true;
				for (auto & pcSelItem : pcSnapItem->vcItems) {
					m_pcWindow->GetHighlightControl().Highlight(pcSelItem, cHighlightOptions, bInRemoveExisting);
					bInRemoveExisting = false;
				}

				if (0 < m_cOldHighlightSelection.GetCount()) {
					m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection, cHighlightOptions);
				}

				m_pcWindow->GetBaseView()->SetSuppressUpdate(false);

				m_pcWindow->Update();

				return HLISTENER_PASS_EVENT;
			}
		}
	}

	nMouseMoveTickCount = GetTickCount();
	nTickCount = nMouseMoveTickCount - m_nPrevMouseMoveTickCount;
	m_nPrevMouseMoveTickCount = nMouseMoveTickCount;
	TRACE(L"SelectByPoint Complete, Tick: %d\n", nTickCount);

	bool bForceUpdate = false;

	// 	새롭게 선택된 Selection Result에서 Line만 남기도록 한다.
	// 	m_cNewHighlightSelection.LeaveType((DWORD)H3DF::Type::LineKey);

	// 추가된것이 있는 경우에 Count를 검사해서 5개까지만 남기도록 한다.
	if (true == m_cHighlightSelection.Union(cSelections)) {
		if (m_nTotalSnapItemCount < m_cHighlightSelection.GetCount()) {
			m_cHighlightSelection.SetSize(m_nTotalSnapItemCount);
		}
	}

	nMouseMoveTickCount = GetTickCount();
	nTickCount = nMouseMoveTickCount - m_nPrevMouseMoveTickCount;
	m_nPrevMouseMoveTickCount = nMouseMoveTickCount;

	if (0 < cSelections.GetCount()) {
		// Object Snap Point를 계산한다.
		CalculationObjectSnapPoint(cSelections);

		// Snap Item을 그린다.
		DrawSnapItems();

		if (false == m_vSnapItems.empty()) {
			bForceUpdate = true;
		}
	}

	// m_pcWindow->GetBaseView()->SetSuppressUpdate(false);

	nMouseMoveTickCount = GetTickCount();
	nTickCount = nMouseMoveTickCount - m_nPrevMouseMoveTickCount;
	m_nPrevMouseMoveTickCount = nMouseMoveTickCount;
	TRACE(L"Update Start, Tick: %d\n", nTickCount);

	if (true == bForceUpdate) {
		//m_pcWindow->Update();
		m_pcWindow->GetBaseView()->ForceUpdate();

		nMouseMoveTickCount = GetTickCount();
		nTickCount = nMouseMoveTickCount - m_nPrevMouseMoveTickCount;
		m_nPrevMouseMoveTickCount = nMouseMoveTickCount;
		TRACE(L"ForceUpdate, Tick: %d\n", nTickCount);
	}
	else {
		m_pcWindow->Update();

		nMouseMoveTickCount = GetTickCount();
		nTickCount = nMouseMoveTickCount - m_nPrevMouseMoveTickCount;
		m_nPrevMouseMoveTickCount = nMouseMoveTickCount;
		TRACE(L"Update, Tick: %d\n", nTickCount);
	}

	return HLISTENER_PASS_EVENT;
}

// 2.1 Dynamic Highlight 처리
bool KERNEL::Operator::HighlightObjectSnapPrivate::DoDynamicHighlighting(WindowPoint cMousePoint, SelectionResults & cSelections)
{
	DEBUG_VALID(m_pcWindow);

	BaseView * pcView = m_pcWindow->GetBaseView();
	DEBUG_VALID(pcView);

	if (pcView->GetSuppressUpdateTick() || pcView->GetSuppressUpdate() || !pcView->GetModel()->GetFileLoadComplete()) {
		return false;
	}
	
	bool bNeedUpdate = true;

	// SetBias(Selection::Bias::Lines)함수는 Line을 우선적으로 선택하도록 한다. 선택후에는 Sort함수를 통해서 Shell값과 Z값으로 정렬된다.
	// 전달되는 값에는 Line이 빠지지 않고 전달된다. Line은 Polyline을 함께 포함하고 있음.


/*	HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetOverwriteKey()); {
		//HC_Set_Selectability("everything = off, lines = on");
// 		HC_Set_Rendering_Options("attribute lock = (line weight)");
 		HC_Set_Line_Weight(1.9);
// 		HC_Set_Line_Weight(SELECT_EDGE_WEIGHT);
	} HC_Close_Segment(); */

	float fProximity = 0.2f;
	SelectionOptionsKit cSelectOption;
	cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(15).SetProximity(0.2f); // .SetBias(Selection::Bias::Lines);
	SelectionResults cHighlightSelection;
	size_t nResult = m_pcWindow->GetSelectionControl().SelectByPoint(cMousePoint, cSelectOption, cHighlightSelection);

// 	HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetOverwriteKey()); {
// 		HC_Set_Selectability("everything = off, faces = on");
// 		HC_UnSet_Line_Weight();
// 	} HC_Close_Segment();

// 	cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(3).SetProximity(0.1f);// .SetBias(Selection::Bias::None);
// 	nResult += m_pcWindow->GetSelectionControl().SelectByPoint(cMousePoint, cSelectOption, cHighlightSelection);

	// 선택된 요소가 없는 경우 Deselect All을 하고 Update를 한다.
	if(0 == nResult) {
		if(0 < m_cOldHighlightSelection.GetCount()) {
			m_pcWindow->GetBaseView()->GetHighlightSelection()->DeSelectAll();
			m_pcWindow->GetBaseView()->ForceUpdate();
			m_cOldHighlightSelection.Reset();
		}

		return false;
	}

	// 선택결과를 Z값으로 Sort한다.
	cHighlightSelection.Sort();

	// 첫번째 요소를 저장한다.
	SelectionItem * pcFrontItem = nullptr;
	if(0 < nResult) {
		pcFrontItem = cHighlightSelection.Front();
	}

	// #Todo: Selection Filter를 적용해야 함.

	// 2개 이상의 요소가 선택된 경우 처리한다.
	if (1 < cHighlightSelection.GetCount()) {
		// ----- 선택된 요소에서 Line이나 Edge를 우선적으로 찾도록 한다. -----
		DEBUG_VALID(pcFrontItem);

		// 1. 첫번째 요소가 Shell인 경우 다음 요소에서 Line을 찾는다. 
		if (H3DF::Type::ShellKey == pcFrontItem->Type()) {
			WorldPoint cFaceWordlPoint;
			WindowPoint cFaceWindowPoint;
			pcFrontItem->ShowSelectionPosition(cFaceWordlPoint);
			pcFrontItem->ShowSelectionPosition(cFaceWindowPoint);

			SelectionResultsIterator cIter = cHighlightSelection.GetIterator();
			// 첫번째 요소 다음을 선택한다.
			cIter.Next();

			while (true == cIter.IsValid()) {
				SelectionItem * pcNextItem = cIter.GetItem();
				// Line을 선택한다. Line을 우선적으로 선택하기 위한 것임.
				// Line과 첫번째 Shell과 선택점과의 Z값을 비교한다. 값의 공차가 Proximity보다 작은 경우 Line을 선택한다.
				if (H3DF::Type::LineKey == pcNextItem->Type()) {
					WorldPoint cLineWordlPoint;
					WindowPoint cLineWindowPoint;
					pcNextItem->ShowSelectionPosition(cLineWordlPoint);
					pcNextItem->ShowSelectionPosition(cLineWindowPoint);

					TRACE(L"Face Line Distance: %f, %f\n", fabs(cFaceWindowPoint.z - cLineWindowPoint.z), cLineWordlPoint.DistanceWith(cFaceWordlPoint));

					// 첫번째에 Shell이 선택되고 다른 Item에서 Line이 공차내로 들어오면 Shell 대신 Line을 선택하고 끝낸다.
					if(0.001 > fabs(cFaceWindowPoint.z - cLineWindowPoint.z)) {
						if(2.0 > cLineWordlPoint.DistanceWith(cFaceWordlPoint)) {
							pcFrontItem = pcNextItem;
							break;
						}
					}
				}
				cIter.Next();
			}
		}
	}

	m_pcWindow->GetBaseView()->GetHighlightSelection()->DeSelectAll();

	H3DF::HighlightOptionsKit cOption;
	if(0 < cHighlightSelection.GetCount()) {
		if(H3DF::Type::LineKey == pcFrontItem->Type()) {
			float fLineWeight = 3.0;
			HC_KEY nHighlightSelectionKey = m_pcWindow->GetBaseView()->GetHighlightSelection()->GetSelectionSegment();
			HC_Open_Segment_By_Key(nHighlightSelectionKey); {
				HC_Set_Line_Weight(fLineWeight);
			} HC_Close_Segment();
		}
		else {
			float fLineWeight = 1.0;
			HC_KEY nHighlightSelectionKey = m_pcWindow->GetBaseView()->GetHighlightSelection()->GetSelectionSegment();
			HC_Open_Segment_By_Key(nHighlightSelectionKey); {
				HC_Set_Line_Weight(fLineWeight);
			} HC_Close_Segment();
		}

		cSelections.PushBack(new SelectionItem(*pcFrontItem));
		m_pcWindow->GetHighlightControl().Highlight(*pcFrontItem, cOption);
		bNeedUpdate = true;
	}

	m_cOldHighlightSelection = cSelections;

	return true;
}

// 기존값과 다른 값이 입력되면 확인해서 삭제하거나 추가한다.
void KERNEL::Operator::HighlightObjectSnapPrivate::SetObjectSnapMode(DWORD nInSnapMode) 
{
	if (m_nOSnapMode == nInSnapMode) {
		return;
	}

	m_nOSnapMode = nInSnapMode;

	// Osnap type이 없는 경우 삭제
	bool bFindEraseType = false;
	for (auto pcItem : m_vSnapItems) {
		auto cIterator = pcItem->vcSnapPoints.begin();

		while (cIterator != pcItem->vcSnapPoints.end()) {
			SnapPoint & cSnapPoint = *cIterator;

			// & 연산은 값이 포함되어 있지 않으면 0이 된다.
			// 입력된 Snap Mode에 포함되어 있지 않으면 삭제한다.
			if (0 == (nInSnapMode & (DWORD) cSnapPoint.eType)) {
				cIterator = pcItem->vcSnapPoints.erase(cIterator);
				bFindEraseType = true;
			}
			else {
				++cIterator;
			}
		}
	}

	if (true == bFindEraseType) {
		DrawSnapItems();
		// m_pcWindow->GetBaseView()->Update();
	}
}

//== 1. Object Snap 계산 ============================================================================

// 1. 주어진 Selection Object를 이용해서 연관된 Object Snap Point를 계산한다.
// 이 함수에서 개별요소의 Object Snap를 구하고, 연관된 요소들의 Object Snap를 구한다. 구하는 Object Snap은 각각의 
// 요소에서 End, Mid, Near, Center등을 구하고 연관된 Entity에서 Intersection, Perpendicular, Tangent 등을 구한다.
// 구해진 값은 m_aSnapItems에 저장된다. SnapItem에는 연관된 Key값, Point, Snap Type등이 저장된다.
void KERNEL::Operator::HighlightObjectSnapPrivate::CalculationObjectSnapPoint(H3DF::SelectionResults & cInItems)
{
	TRACE(L"ObjectSnapPrivate::Items Count: %d\n", m_vSnapItems.size());

	// 단일 Object Snap Point를 계산한다. 이 경우 첫번째 Item만 처리한다.
	SelectionItem * pcItem = cInItems.Front();

	if (nullptr != pcItem) {
		WindowPoint cWindowPoint;
		pcItem->ShowSelectionPosition(cWindowPoint);

		Key cKey;
		pcItem->ShowSelectedItem(cKey);
		H3DF::Type eType = cKey.Type();

		// Line Key 처리
		if (H3DF::Type::LineKey == eType) {
			// 사전 선택된 Object Snap Point 삭제
			//ResetSnapItem();
			CalculationLienObjectSnapPoint(pcItem, cWindowPoint);
		}
	}

	// #Temp
	return;

	//----- 상호간의 Object Snap Point를 계산한다. -----

	SelectionResultsIterator cIter = cInItems.GetIterator();

	// 제일 첫번째 Item을 메인으로 해서 계산을 진행한다.

	if (false == cIter.IsValid()) { // 정상적인 상태인지 확인.
		return;
	}

	pcItem = cIter.GetItem();

	KeyPath cPath;
	pcItem->ShowPath(cPath);

	Matrix cMatrix;
	cPath.ShowNetModellingMatrix(cMatrix);

	Key cSelection;
	if (false == pcItem->ShowSelectedItem(cSelection)) {
		return;
	}

	// 다음 Item을 가져온다.
	cIter.Next();

	while (cIter.IsValid()) {
		SelectionItem * pcNextItem = cIter.GetItem();
		
		KeyPath cNextPath;
		pcNextItem->ShowPath(cNextPath);
		
		Matrix cNextMatrix;
		cNextPath.ShowNetModellingMatrix(cNextMatrix);

		Key cNextSelection;
		if (true == pcNextItem->ShowSelectedItem(cNextSelection)) {
			// 다른 Line과 관련된 Object Snap point를 계산한다.
			if (H3DF::Type::LineKey == cSelection.Type() && H3DF::Type::LineKey == cNextSelection.Type()) {
				CalculationLienAndLineObjectSnapPoint(pcItem, pcNextItem, cMatrix, cNextMatrix);
			}
		}

		// 다음 Item을 가져온다.
		cIter.Next();
	}
}

//== 2. 단일 Geometry Object Snap 계산 ==============================================================

// 2-1. Line Object Snap 계산 (EndPoint, MidPoint, NearPoint를 계산)
bool KERNEL::Operator::HighlightObjectSnapPrivate::CalculationLienObjectSnapPoint(const SelectionItem * pcInSelectionItem, const WindowPoint & cInPoint)
{
	Key cKey;
	pcInSelectionItem->ShowSelectedItem(cKey);

	if (H3DF::Type::LineKey != cKey.Type()) {
		return false;
	}

	LineKey cLine = LineKey(cKey);

	KeyPath cPath;
	pcInSelectionItem->ShowPath(cPath);

	Matrix cMatrix;
	cPath.ShowNetModellingMatrix(cMatrix);

	WorldPointArray aPoints;
	cLine.ShowPoints(aPoints);

	size_t nCount = aPoints.size();

	if (1 >= nCount) {
		return false;
	}

	// 신규 Snap Item을 생성
	auto * psSnapItem = new SnapItem();

	// Selection Item
	psSnapItem->vcItems.push_back(*pcInSelectionItem);

	if (false == AddSnapItems(psSnapItem)) {
		delete psSnapItem;
		return false;
	}

	// End Point 처리
	if (m_nOSnapMode & (DWORD) OSnap::Type::EndPoint) {
		Point cSP, cEP;
		if (true == cLine.GetEndPoint(cSP, cEP)) {
			cSP = cMatrix.Transform(cSP);
			cEP = cMatrix.Transform(cEP);

			AddSnapItem(psSnapItem, cSP, OSnap::Type::EndPoint);

			if (false == cSP.Equals(cEP)) {
				AddSnapItem(psSnapItem, cEP, OSnap::Type::EndPoint);
			}
		}
	}

	// Mid Point 처리
	if (m_nOSnapMode & (DWORD) OSnap::Type::MidPoint) {
		Point cMP;
		if (true == cLine.GetMidPoint(cMP)) {
			cMP = cMatrix.Transform(cMP);
			AddSnapItem(psSnapItem, cMP, OSnap::Type::MidPoint);
		}
	}

	// Near Point 처리
	// Near Point는 별도값으로 저장해야 계속해서 나타나는 문제를 해결할 수 있다.
	if (m_nOSnapMode & (DWORD) OSnap::Type::NearPoint) {
		WorldPoint cNearPoint;
		if (true == cLine.NearPoint(*m_pcWindow, cMatrix, cInPoint, cNearPoint)) {
			//AddSnapItem(psSnapItem, cNearPoint, Type::NearPoint);
		}
	}

	CircleKit cCircle;
	if (m_nOSnapMode & (DWORD) OSnap::Type::Center) {
		if (true == H3DF::Math::GetCircle(aPoints, cCircle)) {
			Point cCenter;
			cCircle.ShowCenter(cCenter);
			cCenter = cMatrix.Transform(cCenter);
			AddSnapItem(psSnapItem, cCenter, OSnap::Type::Center);

			return true;
		}
	}

	return true;
}

//== 3. 2개의 Geometry Object Snap 계산 =============================================================

// 3-1. Line & Line 관련 Object Snap을 계산, Intersection
void KERNEL::Operator::HighlightObjectSnapPrivate::CalculationLienAndLineObjectSnapPoint(const SelectionItem * pcInItems1, const SelectionItem * pcInItems2, 
	const MatrixKit & cMatrix1, const MatrixKit & cMatrix2)
{
	if (!(m_nOSnapMode & (DWORD) OSnap::Type::Intersection) && !(m_nOSnapMode & (DWORD) OSnap::Type::Perpendicular)) {
		return;
	}

	if (*pcInItems1 == *pcInItems2) {
		return;
	}

	Key cSelection1;
	if (false == pcInItems1->ShowSelectedItem(cSelection1)) {
		return;
	}

	Key cSelection2;
	if (false == pcInItems2->ShowSelectedItem(cSelection2)) {
		return;
	}

	LineKey cLine1 = LineKey(cSelection1);
	LineKey cLine2 = LineKey(cSelection2);

	if (true == cLine1.IsCoincident(cLine2, cMatrix1, cMatrix2)) {
		return;
	}

	PointArray aInterPoints;
	if (true == cLine1.GetIntersectionPoint(cLine2, cMatrix1, cMatrix2, aInterPoints)) {
		auto * pcSnapItem = new SnapItem();
		pcSnapItem->vcItems.push_back(*pcInItems1);
		pcSnapItem->vcItems.push_back(*pcInItems2);

		// 찾아온 교차점을 SnapItem에 추가한다.
		//for (size_t nIndex = 0; nIndex < aIntersectionPoints.size(); nIndex++) {
		for (auto cPoint : aInterPoints) {
			AddSnapItem(pcSnapItem, cPoint, OSnap::Type::Intersection);
		}

		if (false == AddSnapItems(pcSnapItem)) {
			delete pcSnapItem;
		}
	}
}

//== Object Snap Point를 그리는 함수 ==================================================================

void KERNEL::Operator::HighlightObjectSnapPrivate::DrawSnapItems()
{
	if (0 < m_vSnapItems.size())
	{
		CamerInformation cCameraInfo;
		ShowCameraInformation(m_fSnapRadius, cCameraInfo);

		SegmentKeyPrivate::ForcedOpen(m_cSnapPointSegment); {
			HC_Flush_Contents(".", "geometry, segment");

			m_cSnapPointSegment.SetModellingMatrix(cCameraInfo.cMatrix);

			for (auto pcItem : m_vSnapItems) {
				for (auto & cSnapPoint : pcItem->vcSnapPoints) {
					DrawSnapPoint(cSnapPoint, cCameraInfo);
				}
			}
		} SegmentKeyPrivate::ForcedClose(m_cSnapPointSegment);
		

		m_pcWindow->GetBaseView()->ForceUpdate();
	}
}

void KERNEL::Operator::HighlightObjectSnapPrivate::DrawSnapPoint(Operator::HighlightObjectSnapPrivate::SnapPoint & cSnapPoint, CamerInformation & cInCameraInfo)
{
	m_cSnapPointSegment.Open(); {
		m_cSnapPointSegment.SetModellingMatrix(cInCameraInfo.cMatrix);

		Point2D cDropPoint = cSnapPoint.cPoint.DropPoint(cInCameraInfo.cOrigin, cInCameraInfo.cXAixs, cInCameraInfo.cYAixs);
		DrawSnapPoint(cDropPoint, cSnapPoint.eStatus, cSnapPoint.eType, cInCameraInfo.dObjectSnapRadius);
	} m_cSnapPointSegment.Close();
}

void KERNEL::Operator::HighlightObjectSnapPrivate::DrawSnapPoint(Point2D center, Status eInStatus, OSnap::Type eInType, double dUnit)
{
	using namespace Painter;

	// pixel to world
	double fontSize = PixelToWorld(TheEnvironment.General.FontSize * TheSession.DpiScale);
	//:WARNING - replace dUnit
	dUnit = fontSize * 0.5;

	const COLORREF PointBackColor = RGB(0xFD, 0xF4, 0xDC);
	const COLORREF PointWireColor = RGB(0x1F, 0x1E, 0x1C);
	const COLORREF TooltipTextColor = RGB(0xF0, 0xF0, 0xD5);
	const COLORREF TooltipBackColor = RGB(0x43, 0x43, 0x43);
	const COLORREF TooltipEdgeColor = RGB(0x64, 0x64, 0x64);

	Point position(center);

	HC_Open_Segment("inner");
	{
		Segment::SetVisibility("edges", false);
		Segment::SetColor("faces", PointBackColor);

		Circle::Create(position, dUnit, true);

		HC_Open_Segment("wire");
		{
			Segment::SetColor("faces", PointWireColor);

			double inner = dUnit * 0.5;
			double outer = dUnit;
			Figure::CreateDonut(position, inner, outer);
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();

	if (eInStatus != Status::Selected) {
		return;
	}

	HC_Open_Segment("outer");
	{
		Segment::SetVisibility("edges", false);
		Segment::SetColor("faces", PointBackColor, 0.5);

		double inner = dUnit;
		double outer = dUnit * 2;
		Figure::CreateDonut(position, inner, outer);
	}
	HC_Close_Segment();

	const wchar_t * pText = nullptr;

	switch (eInType) {
		case OSnap::Type::EndPoint:			pText = L"End Point|끝점";			break;
		case OSnap::Type::MidPoint:			pText = L"Mid Point|중점";			break;
		case OSnap::Type::NearPoint:		pText = L"Near Point|근점";			break;
		case OSnap::Type::Center:			pText = L"Center Point|중심점";		break;
		case OSnap::Type::Intersection:		pText = L"Intersection Point|교차점";	break;
		default:
			ASSERT(FALSE);
			return;
	}

	HC_Open_Segment("snap name");
	{
		Segment::SetColor("text", TooltipTextColor);
		Font::SetName(TheEnvironment.General.FontName());
		Font::SetSize(fontSize, "oru");
		Font::SetRenderer("truetype");
		Font::SetAlignment(Font::EPivot::MiddleCenter);

		//:TODO - text position in window
		double textOffset = dUnit * 5;
		position.y += textOffset;
		CString text = TheEnvironment.General.Local(pText);
		Text::Create(position, text);

		//:WARNING - for calculating text extent
		Font::SetTransform();
			float width, height;
			Text::GetExtent(text, width, height);
		Font::SetTransform(false);

		HC_Open_Segment("frame");
		{
			Segment::SetEdgeWeight(PixelToWorld(1));
			Segment::SetColor("faces", TooltipBackColor);
			Segment::SetColor("edges", TooltipEdgeColor);

			double padding = dUnit * 3;
			Point size(width, height + padding);
			H3DF::Point p1(position.x - size.x / 2, position.y + size.y / 2);
			H3DF::Point p2(position.x + size.x / 2, position.y - size.y / 2);

			Figure::CreateObround(p1, p2);
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();
}

double KERNEL::Operator::HighlightObjectSnapPrivate::PixelToWorld(double unit)
{
	PixelPoint pixel1;
	PixelPoint pixel2(unit, 0, 0);
	WorldPoint world1(*m_pcWindow, pixel1);
	WorldPoint world2(*m_pcWindow, pixel2);
	Vector vector = world2 - world1;

	return vector.Length();
}

bool KERNEL::Operator::HighlightObjectSnapPrivate::ShowCameraInformation(float fInRadius, CamerInformation & cOutInfo)
{
	DEBUG_VALID(m_pcWindow);

	const WindowKeyPrivate * pcWindowKeyPrivate = static_cast<const WindowKeyPrivate *>(m_pcWindow->GetImpl());

	SegmentKey cSecne(pcWindowKeyPrivate->GetSceneKey());

	CameraKit cCamera;
	cSecne.ShowCamera(cCamera);
	cCamera.ShowMatrix(cOutInfo.cMatrix);

	cOutInfo.cXAixs = cOutInfo.cMatrix.XAxis();
	cOutInfo.cYAixs = cOutInfo.cMatrix.YAxis();
	cOutInfo.cOrigin = cOutInfo.cMatrix.Origin();

	PixelPoint cPixelPoint1;
	PixelPoint cPixelPoint2(fInRadius, 0, 0);
	WorldPoint cWorldPoint1(*m_pcWindow, cPixelPoint1);
	WorldPoint cWorldPoint2(*m_pcWindow, cPixelPoint2);
	Vector cVector = cWorldPoint2 - cWorldPoint1;
	cOutInfo.dObjectSnapRadius = cVector.Length();

	return true;
}

//== Utility Functions =============================================================================
bool KERNEL::Operator::HighlightObjectSnapPrivate::AddSnapItems(SnapItem * psInSnapItem)
{
	if (nullptr == psInSnapItem) {
		return false;
	}

	// 신규 Snap Item과 기존 Item의 중복을 검사한다.
	for (auto pcSelItem : m_vSnapItems) {
		if(*pcSelItem == *psInSnapItem) {
			return false;
		}
	}

	// 5개 이상의 SnapItem을 추가하려면 처음부분을 삭제한다.
	if (m_nTotalSnapItemCount == m_vSnapItems.size()) {
		auto pcSnapItem = m_vSnapItems.front();
		delete pcSnapItem;
		m_vSnapItems.erase(m_vSnapItems.begin());
	}

	m_vSnapItems.push_back(psInSnapItem);

	return true;
}

bool KERNEL::Operator::HighlightObjectSnapPrivate::AddSnapItem(SnapItem * psInSnapItem, Point cInSnapPoint, OSnap::Type eInType)
{
	Operator::HighlightObjectSnapPrivate::SnapPoint cSnapPoint;

	cSnapPoint.cPoint = cInSnapPoint;
	cSnapPoint.eType = eInType;

	psInSnapItem->vcSnapPoints.push_back(cSnapPoint);

	return true;
}

void KERNEL::Operator::HighlightObjectSnapPrivate::ClearSnapItems(bool bUpdate)
{
	m_cSnapPointSegment.Open();
	{
		HC_Flush_Contents(".", "geometry, segment");
	}
	m_cSnapPointSegment.Close();

	if (true == bUpdate) {
		m_pcWindow->GetBaseView()->Update();
	}
}

void KERNEL::Operator::HighlightObjectSnapPrivate::ResetSnapItem()
{
	// m_aSnapItems을 삭제
	for (auto pcSnapItem : m_vSnapItems) {
		delete pcSnapItem;
	}

	m_vSnapItems.clear();
}

#undef TheEnvironment
#undef TheSession
