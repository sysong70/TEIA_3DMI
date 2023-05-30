#include "StdAfx.h"

#include "3DF.Window.h"
#include "3DF.BaseView.h"

#include "3DF.Line.h"
#include "3DF.Circle.h"
#include "3DF.Point.h"

#include "3DF.Math.Matrix.h"

#include "3DF.Camera.h"
#include "3DF.Color.h"
#include "3DF.Material.h"

#include "3DF.Visibility.h"
#include "3DF.VisualEffects.h"

#include "3DF.Operator.ObjectSnap.h"
#include "Private/3DF.SelectionPrivate.h"
#include "../3DF.Signal.Connector.h"

#include <Common_Define.h>

#include <HTools.h>
#include <HBaseView.h>
#include <HSelectionSet.h>
#include <HMarkupManager.h>
#include <HEventManager.h>
#include <HConstantFrameRate.h>


#include "3DF.Painter.h"
#include "3DF.Facility.AppOptions.h"

USING_3DF_NAMESPACE

#define TheEnvironment TheAppOptions.Preference.Environment
#define TheSession TheAppOptions.Preference.Session

Operator::ObjectSnap::ObjectSnap(WindowKey * pcWindow)
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
}

int Operator::ObjectSnap::NoButtonDownAndMove(HEventInfo & cInEvent)
{
	PixelPoint cMousePoint(cInEvent.GetMousePixelPos());

/*
	DWORD nMouseMoveTickCount = GetTickCount();
	DWORD nTickCount = nMouseMoveTickCount - m_nPrevMouseMoveTickCount;
	m_nPrevMouseMoveTickCount = nMouseMoveTickCount;
*/

	float fDist = m_cPrevPoint.DistanceWith(cMousePoint);
	m_cPrevPoint = cMousePoint;

	//TRACE(L"ObjectSnap::NoButtonDownAndMove, Tick: %d, Dist: %f\n", nTickCount, fDist);

	// 같은 Mouse Point가 계속 들어오는 경우는 처리하지 않는다.
	if (3 < fDist || 0 == fDist) {
		return HLISTENER_PASS_EVENT;
	}

	TRACE(L"ObjectSnap::NoButtonDownAndMove, Dist: %f\n", fDist);

// 	if (m_nSelectPickCount > nTickCount) {
// 		return HLISTENER_PASS_EVENT;
// 	}

	CamerInformation cCameraInfo;
	ShowCameraInformation(m_fSnapRadius, cCameraInfo);

	// Event에서 들어온 Mouse 위치를 이용해서 Snap Point가 선택된 경우 (주어진 Pixel 범위내에 있을 때), 
	// Snap Point에 선택 Flag을 주어서 선택된 효과를 주도록 한다.
// 	float fMinDist = FLT_MAX;
// 	ObjectSnap::SnapItem * pcMinSnapItem = nullptr;
	for (auto & pcSnapItem : m_vSnapItems) {

		if (Type::NearPoint == pcSnapItem->eType) {
			continue;
		}

		PixelPoint cPixelPoint(*m_pcWindow, pcSnapItem->cPoint);

		double dDist = cPixelPoint.DistanceWith(cMousePoint);

		if (15 > dDist) {
			pcSnapItem->eStatus = ObjectSnap::Status::Selected;

			// 기존에 선택된 Snap Point가 있으면 삭제한다.	
			m_cSnapPointSegment.Flush(Search::Type::Segment);

			// 기존 선택 요소 Unhighlight
	// 			if (0 < m_cOldHighlightSelection.GetCount()) {
	// 				m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection);
	// 			}

			TRACE(L"1st DrawSnapItem, %d\n", (int)pcSnapItem->eType);
			DrawSnapItem(pcSnapItem, cCameraInfo);

			m_pcWindow->Update();

			//HC_Flush_Contents
			return HLISTENER_PASS_EVENT;
		}

// 		if (dDist < fMinDist) {
// 			fMinDist = dDist;
// 			pcMinSnapItem = pcSnapItem;
// 		}
	}

	// Snap Point가 선택된 경우 처리 (주어진 Pixel 범위내에 있을 때)
	// Snap Point를 그리고 기존 Select Item과 Object Snap Point는 삭제한다.

	SelectionOptionsKit cSelectOption;
	cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(10).SetProximity(0.1f).SetSorting(Selection::Sorting::ZSorting);
	//cSelectOption.SetLevel(Selection::Level::Entity).SetRelatedLimit(10).SetSorting(Selection::Sorting::ZSorting);

	SelectionResults cHighlightSelection;
	size_t nSelectedCount = m_pcWindow->GetSelectionControl().SelectByPoint(cInEvent, cSelectOption, cHighlightSelection);
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
			TDF::Type eType = cSelectKey.Type();

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

			if (TDF::Type::LineKey == eType) {
				TRACE(L"SelectByPoint Line: %d\t[%d]\n", nSelectedCount, cSelectKey.KeyValue());
				vLineSelectedItems.push_back(pcItem);
			}
			else if (TDF::Type::ShellKey == eType) {
				TRACE(L"SelectByPoint Shell: %d\t[%d]\n", nSelectedCount, cSelectKey.KeyValue());		
				vShellSelectedItems.push_back(pcItem);
			}
			else {
				TRACE(L"SelectByPoint: %d\t[%d]\n", nSelectedCount, cSelectKey.KeyValue());		
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
				TRACE(L"Line First Pushback\n");
			}
			// Shell의 선택점과 Line의 선택점이 거의 같은 경우 Line을 선택한다.
			else if (1.0e-2 >fabs(cLinePoint.z - cShellPoint.z)) {
				m_cNewHighlightSelection.PushBack(new SelectionItem(*pcLineItem));
				TRACE(L"Line vs face near Pushback\n");
			}
			else {
				//m_cNewHighlightSelection.PushBack(new SelectionItem(*pcShellItem));
			}
		}
		else if (0 < vLineSelectedItems.size()) {
			// Line만 있는 경우
			m_cNewHighlightSelection.PushBack(new SelectionItem(*vLineSelectedItems[0]));
			TRACE(L"Line only Pushback\n");
		}
		else if (0 < vShellSelectedItems.size()) {
			// Shell만 있는 경우
			m_cNewHighlightSelection.PushBack(new SelectionItem(*vShellSelectedItems[0]));
			TRACE(L"Shell only Pushback\n");
		}
		else {
			// Line과 Shell이 없는 경우
			//m_cNewHighlightSelection.PushBack(new SelectionItem(*cHighlightSelection.GetIterator().GetItem()));
		}
	}
	else {
		TRACE(L"SelectByPoint: %d\n", nSelectedCount);
	}
	
	bool bForceUpdate = false;

	// 	새롭게 선택된 Selection Result에서 Line만 남기도록 한다.
	// 	m_cNewHighlightSelection.LeaveType((DWORD)TDF::Type::LineKey);

	// 추가된것이 있는 경우에 Count를 검사해서 5개까지만 남기도록 한다.
	if (true == m_cHighlightSelection.Union(m_cNewHighlightSelection)) {
		if (5 < m_cHighlightSelection.GetCount()) {
			m_cHighlightSelection.SetSize(5);
		}
	}

	HighlightOptionsKit cHighlightOptions;
	// Update를 하지 않기 위해서 Notification을 끈다.
	cHighlightOptions.SetNotification(false);

	// 선택된 요소가 있고 기존과 다른 경우에만 Highlight를 한다.
	if (0 < m_cNewHighlightSelection.GetCount() && m_cOldHighlightSelection != m_cNewHighlightSelection) {
		TRACE(L"HighlightSelection Count: %d\n", m_cNewHighlightSelection.GetCount());
		m_pcWindow->GetHighlightControl().Highlight(m_cNewHighlightSelection, cHighlightOptions, true);
		bForceUpdate = true;
	}
	else if(0 == m_cNewHighlightSelection.GetCount() && 0 < m_cOldHighlightSelection.GetCount()) {
		// 기존에 선택된 요소가 있고 새로운 요소가 없는 경우에는 기존 요소를 지운다.
		m_pcWindow->GetHighlightControl().Unhighlight(m_cOldHighlightSelection, cHighlightOptions);

		// 화면상의 SnapItem을 지운다.
		if(false == m_vSnapItems.empty()) { 
			ClearSnapItems(false); 
		}
		
		bForceUpdate = true;

		Key cOldKey;
		if (0 < m_cOldHighlightSelection.GetCount()) {
			m_cOldHighlightSelection.Front()->ShowSelectedItem(cOldKey);
		}

		TRACE(L"Unhighlight: %d\n", cOldKey.KeyValue());
	}

	m_cOldHighlightSelection = m_cNewHighlightSelection;

	if (0 < m_cNewHighlightSelection.GetCount()) {
		// Object Snap Point를 계산한다.
		CalculationObjectSnapPoint(m_cHighlightSelection);

		// Snap Item을 그린다.
		DrawSnapItems(false);

		if(false == m_vSnapItems.empty()) {
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
	}*/

	if (true == bForceUpdate) {
		//m_pcWindow->Update();
		m_pcWindow->GetBaseView()->ForceUpdate();
	}

	return HLISTENER_PASS_EVENT;
}

//:TODO - remove later
/*
void HDraw::Test(HBaseView* view, TDF::Matrix& cMatrix, Point2D p1, Point2D p2)
{
	SetView(view);

	HC_Open_Segment("test_draw");
	{

		HC_Set_Modelling_Matrix(cMatrix.m_fData);

		HC_Open_Segment("construct");
		{
			// Remove previous

			HC_Flush_Contents(".", "geometry, segment");

			// Set default settings

			HC_Set_Color("edges = white");
			HC_Set_Color("faces = black");
			HC_Set_Color("lines = white");
			HC_Set_Color("text = white");

			HC_Set_Visibility("edges");
			HC_Set_Visibility("faces");
			HC_Set_Visibility("lines");
			HC_Set_Visibility("text");

			// Draw line

			HC_Open_Segment("world");
			{
				HC_Set_Line_Weight(2);
				HC_Set_Line_Pattern("- -");

				Line::Create(Point(p1), Point(p2));
			}
			HC_Close_Segment();

			// Draw first symbol

			HC_Open_Segment("first");
			{
				HC_Set_Color("edges = black");
				HC_Set_Color("faces = white");

				HC_Set_Edge_Weight(4);
				//:TODO - calculate point or use segment metrix
				double radius = Compute::PixelToWorld(8);
				Circle::Create(Point(p1), radius, false);
			}
			HC_Close_Segment();

			// Draw second symbol

			HC_Open_Segment("second");
			{
				HC_Set_Color("edges = black");
				HC_Set_Color("faces = white");

				HC_Set_Edge_Weight(4);
				//:TODO - calculate point or use segment metrix
				double radius = Compute::PixelToWorld(8);
				Circle::Create(Point(p2), radius, false);
			}
			HC_Close_Segment();

			// Draw text and outer frame

			Point2D center = (p1 + p2) / 2.0f;

			Vector2D cVector = p2 - p1;
			float fAngle = Vector2D::XAxis().CCWAngleWith(cVector);

			// Text 회전각도 조절
			if (90.0f < fAngle && fAngle < 270.0f) {
				fAngle = fAngle + 180.0f;
			}

			TDF::Matrix cRotation;
			cRotation.RotateOffAxis(Vector::ZAxis(), fAngle);
			cRotation.Translate(center.x, center.y, 0.0f);

			HC_Open_Segment("text");
			{
				// 회전 Matrix 적용
				HC_Set_Modelling_Matrix(cRotation.m_fData);

				HC_Set_Edge_Weight(2);

				Font::SetName("arial");
				Font::SetBold();
				Font::SetSize(Compute::PixelToWorld(32), "oru");
				Font::SetRenderer("truetype");
				Font::SetTransform();

				Format value("%.3f mm", Compute::Distance(Point(p1), Point(p2)));
				float width, height;
				Text::GetExtent(value, width, height);

				//:TODO - calculate point or use segment metrix

//                 Point2D cTestCenter;
//                 cTestCenter.x = (cDrop1.x + cDrop2.x) / 2.0f;
//                 cTestCenter.y = (cDrop1.y + cDrop2.y) / 2.0f;

				//Text::Create(cTextCenter, value);
				Text::Create(Point(0, -height / 2, 1.0f), value);

				double offset = Compute::PixelToWorld(8);
				Point2D cFigureCenter = center;
				Point2D cOffset1(-width / 2, height / 2 + offset);
				Point2D cOffset2(width / 2, -height / 2 - offset);

				//:TODO - calculate point or use segment metrix
				//Figure::CreateObround(cFigureCenter + cOffset1, cFigureCenter + cOffset2);
				Figure::CreateObround(Point(-width / 2, height / 2 + offset), Point(width / 2, -height / 2 - offset));
			} HC_Close_Segment();
		} HC_Close_Segment();
	} HC_Close_Segment();

	SetView(nullptr);
}
*/

//== 1. Object Snap 계산 =============================================================================== 

// 1. 주어진 Selection Object를 이용해서 연관된 Object Snap Point를 계산한다.
// 이 함수에서 개별요소의 Object Snap를 구하고, 연관된 요소들의 Object Snap를 구한다. 구하는 Object Snap은 각각의 
// 요소에서 End, Mid, Near, Center등을 구하고 연관된 Entity에서 Intersection, Perpendicular, Tangent등을 구한다.
// 구해진 값은 m_aSnapItems에 저장된다. SnapItem에는 연관된 Key값, Point, Snap Type등이 저장된다.
void Operator::ObjectSnap::CalculationObjectSnapPoint(TDF::SelectionResults & cInItems)
{
	// 단일 Object Snap Point를 계산한다. 이 경우 첫번째 Item만 처리한다.
	SelectionItem * pcItem = cInItems.Front();

	if (nullptr != pcItem) {
		WorldPoint cWorldPoint;
		WindowPoint cWindowPoint;

		pcItem->ShowSelectionPosition(cWorldPoint);
		pcItem->ShowSelectionPosition(cWindowPoint);

		Key cKey;
		pcItem->ShowSelectedItem(cKey);
		TDF::Type eType = cKey.Type();

		KeyPath cPath;
		pcItem->ShowPath(cPath);

		Matrix cMatrix;
		cPath.ShowNetModellingMatrix(cMatrix);
	
		// Line Key 처리
		if (TDF::Type::LineKey == eType) {
			// 사전 선택된 Object Snap Point 삭제
			ResetSnapItem();
			CalculationLienObjectSnapPoint(cKey, cWindowPoint, cMatrix);
		}
	}

	//----- 상호간의 Object Snap Point를 계산한다. -----

	SelectionResultsIterator cIter = cInItems.GetIterator();
	
	// 제일 첫번째 Item을 메인으로 해서 계산을 진행한다.

	if(false == cIter.IsValid()) { // 정상적인 상태인지 확인.
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
			if (TDF::Type::LineKey == cSelection.Type() && TDF::Type::LineKey == cNextSelection.Type()) {
				LineKey cLine = LineKey(cSelection);
				LineKey cNextLine = LineKey(cNextSelection);
 				CalculationLienAndLineObjectSnapPoint(cLine, cNextLine, cMatrix, cNextMatrix);
 			}
		}
		// 다음 Item을 가져온다.
		cIter.Next();
	}
}

//== 2. 단일 Geometry Object Snap 계산 ==============================================================

// 2-1. Line Object Snap 계산 (EndPoint, MidPoint, NearPoint를 계산)
bool Operator::ObjectSnap::CalculationLienObjectSnapPoint(const Key & cInLineKey, const WindowPoint & cInPoint, const MatrixKit & cModelingMatrix)
{
	if (TDF::Type::LineKey != cInLineKey.Type()) {
		return false;
	}

	LineKey cLine = LineKey(cInLineKey);

	WorldPointArray aPoints;
	cLine.ShowPoints(aPoints);

	size_t nCount = aPoints.size();

	if (1 >= nCount) {
		return false;
	}

	CircleKit cCircle;
	if (true == TDF::Math::GetCircle(aPoints, cCircle)) {
		Point cCenter;
		cCircle.ShowCenter(cCenter);
		cCenter = cModelingMatrix.Transform(cCenter);
		AddSnapItem(cLine, cCenter, Type::Center);
		return true;
	}

	// End Point 처리
	Point cSP, cEP;
	if (true == cLine.GetEndPoint(cSP, cEP)) {
		cSP = cModelingMatrix.Transform(cSP);
		cEP = cModelingMatrix.Transform(cEP);
		AddSnapItem(cLine, cSP, Type::EndPoint);
		AddSnapItem(cLine, cEP, Type::EndPoint);
	}

	// Mid Point 처리
	Point cMP;
	if(true == cLine.GetMidPoint(cMP)) {
		cMP = cModelingMatrix.Transform(cMP);
		AddSnapItem(cLine, cMP, Type::MidPoint);
	}

	// Near Point 처리
	WorldPoint cNearPoint;
	if (true == cLine.NearPoint(*m_pcWindow, cModelingMatrix, cInPoint, cNearPoint)) {
		AddSnapItem(cLine, cNearPoint, Type::NearPoint);
	}

	return true;
}

//== 3. 2개의 Geometry Object Snap 계산 =============================================================

// 3-1. Line & Line 관련 Object Snap을 계산, Intersection
void Operator::ObjectSnap::CalculationLienAndLineObjectSnapPoint(LineKey & cLine1, LineKey & cLine2, const MatrixKit & cMatrix1, const MatrixKit & cMatrix2)
{
// 	if (true == cLine1.IsCoincident(cLine2, cMatrix1, cMatrix2)) {
// 		return;
// 	}

	// 교차점 처리
	PointArray aIntersectionPoints;
	if (true == cLine1.GetIntersectionPoint(cLine2, cMatrix1, cMatrix2, aIntersectionPoints)) {
		// 찾아온 교차점을 SnapItem에 추가한다.
		for (size_t nIndex = 0; nIndex < aIntersectionPoints.size(); nIndex++) {
			AddSnapItem(cLine1, aIntersectionPoints[nIndex], Type::Intersection);
		}
	}
}

//== Object Snap Point를 그리는 함수 ==================================================================

void Operator::ObjectSnap::DrawSnapItems(bool bUpdate)
{
	CamerInformation cCameraInfo;
	ShowCameraInformation(m_fSnapRadius, cCameraInfo);

	m_cSnapPointSegment.Open();
	{
		HC_Flush_Contents(".", "geometry, segment");

		m_cSnapPointSegment.SetModellingMatrix(cCameraInfo.cMatrix);

		for (auto pcItem : m_vSnapItems) {
			Point2D cDropPoint = pcItem->cPoint.DropPoint(cCameraInfo.cOrigin, cCameraInfo.cXAixs, cCameraInfo.cYAixs);
			DrawSnapPoint(pcItem, cDropPoint, cCameraInfo.dObjectSnapRadius);
		}
	}
	m_cSnapPointSegment.Close();

	if (true == bUpdate) {
		m_pcWindow->GetBaseView()->Update();
	}
}

void Operator::ObjectSnap::DrawSnapItem(SnapItem * pcInItem, CamerInformation & cInCameraInfo, bool bUpdate)
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

void Operator::ObjectSnap::DrawSnapPoint(SnapItem* pItem, Point2D center, double dUnit)
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

	if (pItem->eStatus != Status::Selected) {
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

	const wchar_t* pText = nullptr;

	switch (pItem->eType) {
	case Type::EndPoint:		pText = L"End Point|끝점";			break;
	case Type::MidPoint:		pText = L"Mid Point|중점";			break;
	case Type::NearPoint:		pText = L"Near Point|근점";			break;
	case Type::Center:			pText = L"Center Point|중심점";		break;
	case Type::Intersection:	pText = L"Intersection Point|교차점";	break;
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
			TDF::Point p1(position.x - size.x / 2, position.y + size.y / 2);
			TDF::Point p2(position.x + size.x / 2, position.y - size.y / 2);

			Figure::CreateObround(p1, p2);
		}
		HC_Close_Segment();
	}
	HC_Close_Segment();
}

double Operator::ObjectSnap::PixelToWorld(double unit)
{
	SegmentKey scene(m_pcWindow->GetSceneKey());

	PixelPoint pixel1;
	PixelPoint pixel2(unit, 0, 0);
	WorldPoint world1(*m_pcWindow, pixel1);
	WorldPoint world2(*m_pcWindow, pixel2);
	Vector vector = world2 - world1;

	return vector.Length();
}

bool Operator::ObjectSnap::ShowCameraInformation(float fInRadius, CamerInformation & cOutInfo)
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

bool Operator::ObjectSnap::AddSnapItem(Key & cInKey, Point cSnapPoint, Type eType)
{
	SnapItem * psSnapItem = new SnapItem();
	psSnapItem->cPoint = cSnapPoint;
	psSnapItem->eType = eType;
	m_vSnapItems.push_back(psSnapItem);

	return true;
}

void Operator::ObjectSnap::ClearSnapItems(bool bUpdate)
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

void Operator::ObjectSnap::ResetSnapItem()
{
	// m_aSnapItems을 삭제
	for (auto pcSnapItem : m_vSnapItems) {
		delete pcSnapItem;
	}

	m_vSnapItems.clear();
}

#undef TheEnvironment
#undef TheSession
