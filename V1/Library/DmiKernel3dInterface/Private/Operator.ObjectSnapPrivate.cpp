#include "StdAfx.h"

#include "Operator.ObjectSnapPrivate.h"

#include <3DF/Window.h>
#include <Private/View.Private.h>

#include <3DF/Line.h>
#include <3DF/Circle.h>
#include <3DF/Point.h>

#include <3DF/Math.Matrix.h>

#include <3DF/Camera.h>
#include <3DF/Color.h>
#include <3DF/Material.h>

#include <3DF/Selection.h>
#include <3DF/Visibility.h>
#include <3DF/VisualEffects.h>

#include <3DF/Private/SelectionPrivate.h>
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

using namespace KERNEL;
using namespace H3DF;

#define TheEnvironment TheAppOptions.Preference.Environment
#define TheSession TheAppOptions.Preference.Session

//== SnapPoint class ===============================================================================

KERNEL::Operator::ObjectSnapPrivate::SnapPoint::SnapPoint(KERNEL::Operator::ObjectSnapPrivate::SnapPoint const & cInThat)
{
	cPoint = cInThat.cPoint;
	eType = cInThat.eType;
	eStatus = cInThat.eStatus;
}

KERNEL::Operator::ObjectSnapPrivate::SnapPoint & KERNEL::Operator::ObjectSnapPrivate::SnapPoint::operator = (KERNEL::Operator::ObjectSnapPrivate::SnapPoint const & cInThat)
{
	cPoint = cInThat.cPoint;
	eType = cInThat.eType;
	eStatus = cInThat.eStatus;

	return *this;
}

//== SnapItem class ================================================================================
// 
// Select Item의 구성 요소가 같은지 확인한다.
bool KERNEL::Operator::ObjectSnapPrivate::SnapItem::operator == (const SnapItem & cInThat) const
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

KERNEL::Operator::ObjectSnapPrivate::ObjectSnapPrivate(H3DF::WindowKey * pcWindow)
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


int KERNEL::Operator::ObjectSnapPrivate::NoButtonDownAndMove(int nFlags, int x, int y)
{
	PixelPoint cMousePoint(x, y, 0);

// 	DWORD nMouseMoveTickCount = GetTickCount();
// 	DWORD nTickCount = nMouseMoveTickCount - m_nPrevMouseMoveTickCount;
// 	m_nPrevMouseMoveTickCount = nMouseMoveTickCount;

	float fDist = m_cPrevPoint.DistanceWith(cMousePoint);
	m_cPrevPoint = cMousePoint;

	//TRACE(L"ObjectSnapPrivate::NoButtonDownAndMove, Tick: %d, Dist: %f\n", nTickCount, fDist);

	// 같은 Mouse Point가 계속 들어오는 경우는 처리하지 않는다.
	if (3 < fDist || 0 == fDist) {
		return HLISTENER_PASS_EVENT;
	}

	//TRACE(L"ObjectSnapPrivate::NoButtonDownAndMove, Dist: %f\n", fDist);

// 	if (m_nSelectPickCount > nTickCount) {
// 		return HLISTENER_PASS_EVENT;
// 	}

	CamerInformation cCameraInfo;
	ShowCameraInformation(m_fSnapRadius, cCameraInfo);

	// Event에서 들어온 Mouse 위치를 이용해서 Snap Point가 선택된 경우 (주어진 Pixel 범위내에 있을 때), 
	// Snap Point에 선택 Flag을 주어서 선택된 효과를 주도록 한다.

// 	float fMinDist = FLT_MAX;
// 	ObjectSnapPrivate::SnapItem * pcMinSnapItem = nullptr;

	// DrawSnapItems(false);

	m_pcWindow->GetBaseView()->SetSuppressUpdate(true);

	// 기존에 선택된 Snap Point가 있으면 삭제한다. Segment를 Flush한다.
	m_cSnapPointSegment.Flush(Search::Type::Segment);

	for (auto & pcSnapItem : m_vSnapItems) {
		for (auto & cSnapPoint : pcSnapItem->vcSnapPoints) {
			cSnapPoint.eStatus = ObjectSnapPrivate::Status::Normal;

			if (OSnap::Type::NearPoint == cSnapPoint.eType) {
				continue;
			}

			DrawSnapPoint(cSnapPoint, cCameraInfo);
		}
	}

	for (auto & pcSnapItem : m_vSnapItems) {
		for (auto & cSnapPoint : pcSnapItem->vcSnapPoints) {
			if (OSnap::Type::NearPoint == cSnapPoint.eType) {
				continue;
			}

			PixelPoint cPixelPoint(*m_pcWindow, cSnapPoint.cPoint);

			double dDist = cPixelPoint.DistanceWith(cMousePoint);

			if (15 > dDist) {
				cSnapPoint.eStatus = ObjectSnapPrivate::Status::Selected;

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

	// Snap Point가 선택된 경우 처리 (주어진 Pixel 범위내에 있을 때)
	// Snap Point를 그리고 기존 Select Item과 Object Snap Point는 삭제한다.

	SelectionOptionsKit cSelectOption;
	cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(10).SetProximity(0.1f).SetSorting(Selection::Sorting::ZSorting);
	//cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(10).SetSorting(Selection::Sorting::ZSorting);

	SelectionResults cHighlightSelection;


	WindowPoint cWindowPoint(*m_pcWindow, cMousePoint);

	Point cLocation;
	cLocation.x = cWindowPoint.x;
	cLocation.y = cWindowPoint.y;

	size_t nSelectedCount = m_pcWindow->GetSelectionControl().SelectByPoint(cLocation, cSelectOption, cHighlightSelection);
	cHighlightSelection.Sort();

	// 신규 선택 요소 저장소는 초기화한다.
	m_cNewHighlightSelection.Reset();

	// Selection된 Item들에서 Windows Point의 Z값을 이용해서 Sort하도록 한다.
	// 1.나오는 Item은 이미 Sorting이 되어 있음.
	// 2.맨앞에 나온 요소가 ShellKey이고, 같은 Z값에 LineKey가 있는 경우 LineKey를 사용하도록 한다.
	if (0 < nSelectedCount) {
		std::vector<SelectionItem *> vLineSelectedItems;
		std::vector<SelectionItem *> vShellSelectedItems;

		SelectionResultsIterator cIter = cHighlightSelection.GetIterator();

		while (true == cIter.IsValid()) {
			SelectionItem * pcItem = cIter.GetItem();

			Key cSelectKey;
			pcItem->ShowSelectedItem(cSelectKey);

			// 나오는 요소의 종류를 확인한다.
			H3DF::Type eType = cSelectKey.Type();

			//m_cNewHighlightSelection.PushBack(new SelectionItem(*pcItem));

			// 첫번째 요소가 ShellKey인 경우 다음 요소와의 거리를 측정해서 공차 범위안에 Line이 있는 경우는 Line을 선택한다.
			// 오차값으로 사용하기에는 값이 너무 크다
/*
			if (Type::ShellKey == eType) {
				m_cNewHighlightSelection.PushBack(new SelectionItem(*pcItem));

				cIter.Next();

				if (true == cIter.IsValid()) {
					SelectionItem * pcNextItem = cIter.GetItem();
					Key cNextSelectKey;
					pcNextItem->ShowSelectedItem(cNextSelectKey);
					// 나오는 요소의 종류를 확인한다.
					Type eType = cNextSelectKey.Type();
					if (Type::LineKey == eType) {
						WorldPoint cItemPoint;
						pcItem->ShowSelectionPosition(cItemPoint);

						WorldPoint cNextItemPoint;
						pcNextItem->ShowSelectionPosition(cNextItemPoint);

						// 두개의 거리를 측정한다.
						double dDistance = cItemPoint.DistanceWith(cNextItemPoint);
						TRACE(L"Dist: %f\n", dDistance);

						m_cNewHighlightSelection.PushBack(new SelectionItem(*pcNextItem));
					}
				}
			}*/

			if (H3DF::Type::LineKey == eType) {
				//TRACE(L"SelectByPoint Line: %d\t[%d]\n", nSelectedCount, cSelectKey.KeyValue());
				vLineSelectedItems.push_back(pcItem);
			}
			else if (H3DF::Type::ShellKey == eType) {
				//TRACE(L"SelectByPoint Shell: %d\t[%d]\n", nSelectedCount, cSelectKey.KeyValue());		
				vShellSelectedItems.push_back(pcItem);
			}
			else {
				//TRACE(L"SelectByPoint: %d\t[%d]\n", nSelectedCount, cSelectKey.KeyValue());		
			}

			cIter.Next();
		}

		// Line과 Shell이 모두 선택된 경우 처리
		if (0 < vLineSelectedItems.size() && 0 < vShellSelectedItems.size()) {
			// Shell과 Line이 같은 Z값에 있는 경우 Line을 사용한다.
			// 1. Shell과 Line의 Z값이 같은지 확인한다.
			// 2. 같은 경우 Line을 사용한다.
			// 3. 다른 경우 Shell을 사용한다.
			SelectionItem * pcLineItem = vLineSelectedItems[0];
			SelectionItem * pcShellItem = vShellSelectedItems[0];

			WindowPoint cLinePoint;
			pcLineItem->ShowSelectionPosition(cLinePoint);

			WindowPoint cShellPoint;
			pcShellItem->ShowSelectionPosition(cShellPoint);

			//TRACE(L"Line Z: %f\tShell Z: %f\t[%f]\n", cLinePoint.z, cShellPoint.z, cLinePoint.z - cShellPoint.z);

			// Line이 가장 앞에 있는 경우 (Windows Point의 Z값이 가장 작은 경우)
			if (cLinePoint.z < cShellPoint.z) {
				m_cNewHighlightSelection.PushBack(new SelectionItem(*pcLineItem));
				//TRACE(L"Line First Pushback\n");
			}
			// Shell의 선택점과 Line의 선택점이 거의 같은 경우 Line을 선택한다.
			else if (1.0e-2 > fabs(cLinePoint.z - cShellPoint.z)) {
				m_cNewHighlightSelection.PushBack(new SelectionItem(*pcLineItem));
				//TRACE(L"Line vs face near Pushback\n");
			}
			else {
				//m_cNewHighlightSelection.PushBack(new SelectionItem(*pcShellItem));
			}
		}
		else if (0 < vLineSelectedItems.size()) {
			// Line만 있는 경우
			m_cNewHighlightSelection.PushBack(new SelectionItem(*vLineSelectedItems[0]));
			//TRACE(L"Line only Pushback\n");
		}
		else if (0 < vShellSelectedItems.size()) {
			// Shell만 있는 경우
			m_cNewHighlightSelection.PushBack(new SelectionItem(*vShellSelectedItems[0]));
			//TRACE(L"Shell only Pushback\n");
		}
		else {
			// Line과 Shell이 없는 경우
			//m_cNewHighlightSelection.PushBack(new SelectionItem(*cHighlightSelection.GetIterator().GetItem()));
		}
	}
	else {
		//TRACE(L"SelectByPoint: %d\n", nSelectedCount);
	}

	bool bForceUpdate = false;

	// 	새롭게 선택된 Selection Result에서 Line만 남기도록 한다.
	// 	m_cNewHighlightSelection.LeaveType((DWORD)H3DF::Type::LineKey);

	// 추가된것이 있는 경우에 Count를 검사해서 5개까지만 남기도록 한다.
	if (true == m_cHighlightSelection.Union(m_cNewHighlightSelection)) {
		if (m_nTotalSnapItemCount < m_cHighlightSelection.GetCount()) {
			m_cHighlightSelection.SetSize(m_nTotalSnapItemCount);
		}
	}

	HighlightOptionsKit cHighlightOptions;
	// Update를 하지 않기 위해서 Notification을 끈다.
	cHighlightOptions.SetNotification(false);

	// 선택된 요소가 있고 기존과 다른 경우에만 Highlight를 한다.
	if (0 < m_cNewHighlightSelection.GetCount() && m_cOldHighlightSelection != m_cNewHighlightSelection) {
		//TRACE(L"HighlightSelection Count: %d\n", m_cNewHighlightSelection.GetCount());
		m_pcWindow->GetHighlightControl().Highlight(m_cNewHighlightSelection, cHighlightOptions, true);
		bForceUpdate = true;
	}
	else if (0 == m_cNewHighlightSelection.GetCount() && 0 < m_cOldHighlightSelection.GetCount()) {
		// 기존에 선택된 요소가 있고 새로운 요소가 없는 경우에는 기존 요소를 지운다.
		m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection, cHighlightOptions);

		// 화면상의 SnapItem을 지운다.
		if (false == m_vSnapItems.empty()) {
			ClearSnapItems(false);
		}

		bForceUpdate = true;

		Key cOldKey;
		if (0 < m_cOldHighlightSelection.GetCount()) {
			m_cOldHighlightSelection.Front()->ShowSelectedItem(cOldKey);
		}

		//TRACE(L"Unhighlight: %d\n", cOldKey.KeyValue());
	}

	m_cOldHighlightSelection = m_cNewHighlightSelection;

	if (0 < m_cNewHighlightSelection.GetCount()) {
		// Object Snap Point를 계산한다.
		CalculationObjectSnapPoint(m_cHighlightSelection);

		// Snap Item을 그린다.
		DrawSnapItems();

		if (false == m_vSnapItems.empty()) {
			bForceUpdate = true;
		}
	}
	/*
		else {
			// Object Snape 등을 지우도록 한다.
			HC_Open_Segment_By_Key(m_pcWindow->GetBaseView()->GetConstructionKey()); {
				HC_Flush_Contents(".", "geometry, segment");
			} HC_Close_Segment();

			if (false == bForceUpdate) {
				// m_pcWindow->GetBaseView()->Update();
			}
		}
	*/

	m_pcWindow->GetBaseView()->SetSuppressUpdate(false);
		
	if (true == bForceUpdate) {
		//m_pcWindow->Update();
		m_pcWindow->GetBaseView()->ForceUpdate();
	}
	else {
		m_pcWindow->Update();
	}

	return HLISTENER_PASS_EVENT;
}

// 기존값과 다른 값이 입력되면 확인해서 삭제하거나 추가한다.
void KERNEL::Operator::ObjectSnapPrivate::SetObjectSnapMode(DWORD nInSnapMode) 
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
void KERNEL::Operator::ObjectSnapPrivate::CalculationObjectSnapPoint(H3DF::SelectionResults & cInItems)
{
	TRACE(L"ObjectSnapPrivate::Items Count: %d\n", m_vSnapItems.size());

	// 단일 Object Snap Point를 계산한다. 이 경우 첫번째 Item만 처리한다.
	SelectionItem * pcItem = cInItems.Front();

	if (nullptr != pcItem) {
		WorldPoint cWorldPoint;
		WindowPoint cWindowPoint;

		pcItem->ShowSelectionPosition(cWorldPoint);
		pcItem->ShowSelectionPosition(cWindowPoint);

		Key cKey;
		pcItem->ShowSelectedItem(cKey);
		H3DF::Type eType = cKey.Type();

		KeyPath cPath;
		pcItem->ShowPath(cPath);

		Matrix cMatrix;
		cPath.ShowNetModellingMatrix(cMatrix);

		// Line Key 처리
		if (H3DF::Type::LineKey == eType) {
			// 사전 선택된 Object Snap Point 삭제
			//ResetSnapItem();
			CalculationLienObjectSnapPoint(pcItem, cWindowPoint);
		}
	}

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
bool KERNEL::Operator::ObjectSnapPrivate::CalculationLienObjectSnapPoint(const SelectionItem * pcInSelectionItem, const WindowPoint & cInPoint)
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
	SnapItem * psSnapItem = new SnapItem();

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
void KERNEL::Operator::ObjectSnapPrivate::CalculationLienAndLineObjectSnapPoint(const SelectionItem * pcInItems1, const SelectionItem * pcInItems2, 
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
		SnapItem * pcSnapItem = new SnapItem();
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

void KERNEL::Operator::ObjectSnapPrivate::DrawSnapItems()
{
	CamerInformation cCameraInfo;
	ShowCameraInformation(m_fSnapRadius, cCameraInfo);

	m_cSnapPointSegment.Open();
	{
		HC_Flush_Contents(".", "geometry, segment");

		m_cSnapPointSegment.SetModellingMatrix(cCameraInfo.cMatrix);

		for (auto pcItem : m_vSnapItems) {
			for (auto & cSnapPoint : pcItem->vcSnapPoints) {
				DrawSnapPoint(cSnapPoint, cCameraInfo);
			}
		}
	}
	m_cSnapPointSegment.Close();
}

/*
void Operator::ObjectSnapPrivate::DrawSnapItem(SnapItem * pcInItem, CamerInformation & cInCameraInfo, bool bUpdate)
{
	m_cSnapPointSegment.Open();
	{
		m_cSnapPointSegment.SetModellingMatrix(cInCameraInfo.cMatrix);

		Point2D cDropPoint = pcInItem->cPoint.DropPoint(cInCameraInfo.cOrigin, cInCameraInfo.cXAixs, cInCameraInfo.cYAixs);
		DrawSnapPoint(pcInItem, cDropPoint, cInCameraInfo.dObjectSnapRadius);

	}
	m_cSnapPointSegment.Close();

	if (true == bUpdate) {
		m_pcWindow->GetBaseView()->Update();
	}
}
*/

void KERNEL::Operator::ObjectSnapPrivate::DrawSnapPoint(Operator::ObjectSnapPrivate::SnapPoint & cSnapPoint, CamerInformation & cInCameraInfo)
{
	m_cSnapPointSegment.Open(); {
		m_cSnapPointSegment.SetModellingMatrix(cInCameraInfo.cMatrix);

		Point2D cDropPoint = cSnapPoint.cPoint.DropPoint(cInCameraInfo.cOrigin, cInCameraInfo.cXAixs, cInCameraInfo.cYAixs);
		DrawSnapPoint(cDropPoint, cSnapPoint.eStatus, cSnapPoint.eType, cInCameraInfo.dObjectSnapRadius);
	} m_cSnapPointSegment.Close();
}

void KERNEL::Operator::ObjectSnapPrivate::DrawSnapPoint(Point2D center, Status eInStatus, OSnap::Type eInType, double dUnit)
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

double KERNEL::Operator::ObjectSnapPrivate::PixelToWorld(double unit)
{
	SegmentKey scene(m_pcWindow->GetSceneKey());

	PixelPoint pixel1;
	PixelPoint pixel2(unit, 0, 0);
	WorldPoint world1(*m_pcWindow, pixel1);
	WorldPoint world2(*m_pcWindow, pixel2);
	Vector vector = world2 - world1;

	return vector.Length();
}

bool KERNEL::Operator::ObjectSnapPrivate::ShowCameraInformation(float fInRadius, CamerInformation & cOutInfo)
{
	if(nullptr == m_pcWindow) {
		return false;
	}

	SegmentKey cSecne(m_pcWindow->GetSceneKey());

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
bool KERNEL::Operator::ObjectSnapPrivate::AddSnapItems(SnapItem * psInSnapItem)
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

bool KERNEL::Operator::ObjectSnapPrivate::AddSnapItem(SnapItem * psInSnapItem, Point cInSnapPoint, OSnap::Type eInType)
{
	Operator::ObjectSnapPrivate::SnapPoint cSnapPoint;

	cSnapPoint.cPoint = cInSnapPoint;
	cSnapPoint.eType = eInType;

	psInSnapItem->vcSnapPoints.push_back(cSnapPoint);

	return true;
}

void KERNEL::Operator::ObjectSnapPrivate::ClearSnapItems(bool bUpdate)
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

void KERNEL::Operator::ObjectSnapPrivate::ResetSnapItem()
{
	// m_aSnapItems을 삭제
	for (auto pcSnapItem : m_vSnapItems) {
		delete pcSnapItem;
	}

	m_vSnapItems.clear();
}

#undef TheEnvironment
#undef TheSession
