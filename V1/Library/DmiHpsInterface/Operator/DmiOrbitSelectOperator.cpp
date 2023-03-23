#include "stdafx.h"

#include "DmiOrbitSelectOperator.h"

#include "../DmiModelHandler.h"
#include "../DmiHpsView.h"
#include "../DmiHpsUtility.h"

#include <Common_Define.h>

#include <sprk.h>

using namespace HPS;

DmiOrbitSelectOperator::DmiOrbitSelectOperator(DmiHpsView * pcHpsView, MouseButtons cInMouseTrigger, ModifierKeys cInModifierTrigger) :
	DmiSelectOperator(cInMouseTrigger, cInModifierTrigger), 
	m_pcHpsView(pcHpsView),
	m_cHighlightOptions(HighlightOptionsKit::GetDefault()),
	m_cDynamicHighlightOptions(HighlightOptionsKit::GetDefault())
{
	m_cSelAreaSelectionOptions.SetRelatedLimit(std::numeric_limits<int>::max())
		.SetLevel(HPS::Selection::Level::Entity)
		//.SetLevel(HPS::Selection::Level::Segment)
		.SetSorting(Selection::Sorting::Default)
		.SetAlgorithm(Selection::Algorithm::Analytic);


// 	HighlightOptionsKit cOptionsKit = HighlightOptionsKit::GetDefault();
// 	cOptionsKit.SetAlgorithm
//	m_cDynamicHighlightOptions.SetAlgorithm(Selection::Algorithm::Visual);

	m_nSelectPickCount = 200;
}

//== View 관련 설정 ==================================================================================

// 1. View를 새롭게 붙는 경우 처리
void DmiOrbitSelectOperator::OnViewAttached(HPS::View const & cInAttachedView)
{
	SetupConstructionSegment();

	SetupPortfolio(cInAttachedView);
}

// 2. View Segemnet의 기본설정 수행.
void DmiOrbitSelectOperator::SetupConstructionSegment()
{
	// 아래 이름은 이미 생성되어 있는 예약어임.
	UTF8 chSegmentName("construction lines");
	if(true == m_bDrawFaces) {
		chSegmentName = "construction polygons";
	}
	
	HPS::SegmentKey cConstructionSegKey = GetAttachedView().GetSegmentKey().Subsegment("construction segments").Subsegment(chSegmentName);

	float fFaceAlpha = 1.0f;
	if(false == DmiHps::GetFrontFaceAlpha(cConstructionSegKey, fFaceAlpha)) {
		fFaceAlpha = 0.2f;
	}

	// ----- Cross Select Segement -----
	m_cCrossScratchSegKey = GetAttachedView().GetSegmentKey().Subsegment("construction segments").Subsegment("construction polygons cross");
	DmiHps::CopySegmentProperties(cConstructionSegKey, m_cCrossScratchSegKey);

	m_cCrossScratchSegKey.GetMaterialMappingControl().SetFaceColor(RGBAColor(0, 0.5, 1, fFaceAlpha)).SetEdgeColor(RGBColor(0, 0.5, 1));
	m_cCrossScratchSegKey.GetDrawingAttributeControl().SetOverlay(Drawing::Overlay::Default);
	m_cCrossScratchSegKey.GetVisualEffectsControl().SetPostProcessEffectsEnabled(false);

	// ----- Include Select Segement -----
	m_cIncludeScratchSegKey = GetAttachedView().GetSegmentKey().Subsegment("construction segments").Subsegment("construction polygons include");
	DmiHps::CopySegmentProperties(cConstructionSegKey, m_cIncludeScratchSegKey);

	m_cIncludeScratchSegKey.GetMaterialMappingControl().SetFaceColor(RGBAColor(0, 1, 0, fFaceAlpha)).SetEdgeColor(RGBColor(0, 1, 0));
	m_cIncludeScratchSegKey.GetDrawingAttributeControl().SetOverlay(Drawing::Overlay::Default);
	m_cIncludeScratchSegKey.GetVisualEffectsControl().SetPostProcessEffectsEnabled(false);

	m_pcHpsView->SetSegementEdgePattern(m_cIncludeScratchSegKey, HPS::LinePattern::Default::Dashed);
}

// 3. Portfolid를 설정 (Highlight 색상등을 설정함)
void DmiOrbitSelectOperator::SetupPortfolio(HPS::View const & cInAttachedView)
{
	PortfolioKey cPortfolioKey = cInAttachedView.GetPortfolioKey();
	HPS::GlyphDefinition cGlyphDefinition;

	if(!cPortfolioKey.ShowGlyphDefinition("plus", cGlyphDefinition)) {
		cGlyphDefinition = cPortfolioKey.DefineGlyph("plus", GlyphKit::GetDefault(Glyph::Default::Plus));
	}

	SegmentKey cModelKey = m_pcHpsView->GetModelKey();

	// Select Highlight Style 생성
	SegmentKey cSelectHighlightStyle = cModelKey.Subsegment();
	cSelectHighlightStyle.GetMaterialMappingControl().
		SetFaceColor(RGBAColor(1, 0.5f, 0, 1)).
		SetEdgeColor(RGBAColor(1, 0.5f, 0, 1));

	//cSelectHighlightStyle.GetDrawingAttributeControl().SetDeferral(2);

	cPortfolioKey.DefineNamedStyle("DMI_HighlightStyle", cSelectHighlightStyle);

	m_cHighlightOptions.SetStyleName("DMI_HighlightStyle");
	m_cHighlightOptions.SetOverlay(HPS::Drawing::Overlay::WithZValues);

	// Dynamic Highlight Style 생성

	MaterialKit cEdgeMaterialKit;
	cEdgeMaterialKit.SetDiffuseColor(DmiHps::GetRgbColor(RGB(255, 0, 0)));
	//cEdgeMaterialKit.SetEmission(DmiHps::GetRgbColor(RGB(0, 255, 0)));
	//cEdgeMaterialKit.SetSpecular(DmiHps::GetRgbColor(RGB(0, 0, 255)));
	//cEdgeMaterialKit.SetGloss(30);
	

	//SegmentKey cDynamicHighlightStyle = cModelKey.Subsegment();
	SegmentKey cDynamicHighlightStyle = cInAttachedView.GetSegmentKey().Subsegment();
	cDynamicHighlightStyle.GetMaterialMappingControl().
		SetFaceColor(RGBAColor(0, 0, 1, 1));
// 		SetEdgeMaterial(cEdgeMaterialKit).
// 		SetLineColor(DmiHps::GetRgbColor(RGB(255, 255, 0)));
	

	//cDynamicHighlightStyle.GetVisibilityControl().SetEdges(false);
	//cDynamicHighlightStyle.GetVisibilityControl().SetLines(true);
	//cDynamicHighlightStyle.GetVisibilityControl().SetAdjacentEdges(true);
	//cDynamicHighlightStyle.GetVisibilityControl().SetGenericEdges(true);
	//cDynamicHighlightStyle.GetVisibilityControl().SetInteriorSilhouetteEdges(true);
	//cDynamicHighlightStyle.GetVisibilityControl().SetPerimeterEdges(true);

	//cDynamicHighlightStyle.GetDrawingAttributeControl().SetGeneralDisplacement(true, -10);
	//cDynamicHighlightStyle.GetDrawingAttributeControl().SetOverlay(HPS::Drawing::Overlay::WithZValues);
	//cDynamicHighlightStyle.GetDrawingAttributeControl().SetDepthRange(0, 0.01f);

	//cDynamicHighlightStyle.GetEdgeAttributeControl().SetWeight(3.0f);

	//cDynamicHighlightStyle.GetEdgeAttributeControl().SetWeight(5.0f);

	cPortfolioKey.DefineNamedStyle("DMI_Dynamic_HighlightStyle", cDynamicHighlightStyle);

	m_cDynamicHighlightOptions.SetStyleName("DMI_Dynamic_HighlightStyle");
	m_cDynamicHighlightOptions.SetOverlay(HPS::Drawing::Overlay::WithZValues);
	// 개별 Polygon이 표시됨.
	//m_cDynamicHighlightOptions.SetSubentityHighlighting(true);
}

//== Mouse Event 처리 ===============================================================================

bool DmiOrbitSelectOperator::OnMouseDown(MouseState const & cInState)
{
	m_nMouseDownTickCount = GetTickCount();

	// Mouse를 Click했을때 처리
	if(true == IsMouseTriggered(cInState))
	{
		m_bOrbitMode = false;
		m_bOperatorActiveFlag = true;
		start_point = cInState.GetLocation();

		HPS::WindowKey cWindowKey = cInState.GetEventSource();
		HPS::PixelPoint cPixelPoint(cWindowKey, start_point);
		m_cMouseDownPoint = cPixelPoint;

		OperatorUtility::ScreenToSphereMousePoint(start_point, start_sphere_pos);
		tracked_touch_ID = -1;

		return true;
	}
// 
// 	if(IsMouseTriggered(cInState) && DmiSelectOperator::OnMouseDown(cInState)) {
// 		HPS::WindowKey window = cInState.GetEventSource();
// 		return HighlightCommon(window, cInState.GetModifierKeys());
// 	}

	return false;
}

bool DmiOrbitSelectOperator::OnMouseUp(MouseState const & cInState)
{
	DWORD nMouseUpTickCount = GetTickCount();
	m_bOrbitMode = false;

	// Select 여부 확인
	if(true == m_bOperatorActiveFlag && !IsMouseTriggered(cInState)) {
		m_bOperatorActiveFlag = false;

		if(true == m_bAreaSelectMode) {
			OnAreaSelectMouseUp(cInState);
			return true;
		}

		HPS::WindowKey cWindowKey = cInState.GetEventSource();
		PixelPoint cPixelPoint(cWindowKey, cInState.GetLocation());

		HPS::Vector cVector = cPixelPoint - m_cMouseDownPoint;
		double dLength = cVector.Length();

		DWORD nTickCount = nMouseUpTickCount - m_nMouseDownTickCount;
		
		// 2 Pixel이하 200 Tick이하에서만 선택하는 것으로 판정한다.
		if(2.0 > dLength && m_nSelectPickCount > nTickCount) {
			// Select 여부 확인
			if(true == DmiSelectOperator::OnMouseUp(cInState)) {
				SelectedItemProcessing();

				HPS::WindowKey cWindow = cInState.GetEventSource();
				return HighlightCommon(cWindow, cInState.GetModifierKeys());
			}
			// Select된 Entity가 없는 경우 처리
			else {
				OnAreaSelectMouseUp(cInState);
			}
		}
	}

	return false;
}

bool DmiOrbitSelectOperator::OnMouseMove(MouseState const & cInState)
{
	// Rigth Button은 Pannig이므로 처리를 하지 않는다.
	MouseButtons cButtons = cInState.GetButtons();
	if(true == cButtons.Right()) {
		return false;
	}

	// Scroll중에는 Mouse Move를 처리하지 않는다.
	MouseEvent cMouseEvent = cInState.GetActiveEvent();
	if(MouseEvent::Action::Scroll == cMouseEvent.CurrentAction) {
		TRACE(L"MouseEvent::Action::Scroll\r\n");
		return false;
	}

	if(true == m_bAreaSelectMode && true == m_bOperatorActiveFlag) {
		HPS::KeyPath cEventPath(cInState.GetEventPath());
		HPS::Point cInnerWindowPoint;
		cEventPath.ConvertCoordinate(HPS::Coordinate::Space::Window, cInState.GetLocation(), HPS::Coordinate::Space::InnerWindow, cInnerWindowPoint);

		return ConstructRectCommon(cInnerWindowPoint);
	}
	else if(true == m_bOperatorActiveFlag && true == IsMouseTriggered(cInState) && true == OrbitCommon(cInState.GetLocation()))
	{
		m_bOrbitMode = true;
		GetAttachedView().Update();
		return true;
	}
	else {
		DmiSelectOperator::OnDaynamicHighlightMouseMove(cInState);
		HPS::WindowKey cWindow = cInState.GetEventSource();
		return DynamicHighlightCommon(cWindow, cInState.GetModifierKeys());
	}

	return false;
}

bool DmiOrbitSelectOperator::OnTouchDown(TouchState const & cInState)
{
	if(!DmiSelectOperator::OnTouchDown(cInState)) {
		return false;
	}

	HPS::WindowKey window = cInState.GetEventSource();
	return HighlightCommon(window, cInState.GetModifierKeys());
}

//== Area Select Function ===========================================================================

// 1. Area Select Mouse Up Event 처리
bool DmiOrbitSelectOperator::OnAreaSelectMouseUp(MouseState const & cInState)
{
	m_cSelAreaTempLineKey.Delete();
	m_cSelAreaTempMarkerKey.Delete();
	m_cSelAreaTempPolygonKey.Delete();

	// 1. Area Select Drag를 시작.
	if(false == m_bAreaSelectMode) {
		HPS::KeyPath cEventPath(cInState.GetEventPath());
		cEventPath.ConvertCoordinate(HPS::Coordinate::Space::Window, cInState.GetLocation(), HPS::Coordinate::Space::InnerWindow, m_cInnerWindowStartPoint);
		m_cWindowStartPoint = cInState.GetLocation();

		m_bAreaSelectMode = true;
		m_bOperatorActiveFlag = true;

		return true;
	}
	// 2. Area Select Drag 종료.
	else {
		m_cCrossScratchSegKey.Flush(HPS::Search::Type::Geometry);

		HPS::KeyPath cEventPath(cInState.GetEventPath());
		HPS::Point cInnerWindowPoint;
		cEventPath.ConvertCoordinate(HPS::Coordinate::Space::Window, cInState.GetLocation(), HPS::Coordinate::Space::InnerWindow, cInnerWindowPoint);

		HPS::Point cNewWindowPoint(cInState.GetLocation());
		HPS::Point window_points[2] = { cNewWindowPoint, m_cWindowStartPoint };
		m_cSelAreaWindowRect = HPS::Rectangle(2, window_points);

		HPS::Point inner_window_points[2] = { cInnerWindowPoint, m_cInnerWindowStartPoint };
		m_cSelAreaInnerWindowRect = HPS::Rectangle(2, inner_window_points);

		m_bSelAreaIsRectValid = true;

		m_bAreaSelectMode = false;
		m_bOperatorActiveFlag = false;

		HPS::WindowKey cWindow = cInState.GetEventSource();
		return AreaSelectCommon(cWindow, cInState.GetModifierKeys());
	}

	return false;
}

// 2. Area Select Rectangle Draw
bool DmiOrbitSelectOperator::ConstructRectCommon(WindowPoint const & cInLocation)
{
	m_cSelAreaTempLineKey.Delete();
	m_cSelAreaTempMarkerKey.Delete();
	m_cSelAreaTempPolygonKey.Delete();

	if(0 < cInLocation.x - m_cInnerWindowStartPoint.x) {
		if(true == m_bDrawFaces) {
			m_cSelAreaTempPolygonKey = InsertPolygon(m_cCrossScratchSegKey, m_cInnerWindowStartPoint.x, m_cInnerWindowStartPoint.y, cInLocation.x, cInLocation.y);
		}
		else {
			m_cSelAreaTempLineKey = InsertRectangle(m_cCrossScratchSegKey, m_cInnerWindowStartPoint.x, m_cInnerWindowStartPoint.y, cInLocation.x, cInLocation.y);
		}
	}
	else {
		if(true == m_bDrawFaces) {
			m_cSelAreaTempPolygonKey = InsertPolygon(m_cIncludeScratchSegKey, m_cInnerWindowStartPoint.x, m_cInnerWindowStartPoint.y, cInLocation.x, cInLocation.y);
		}
		else {
			m_cSelAreaTempLineKey = InsertRectangle(m_cIncludeScratchSegKey, m_cInnerWindowStartPoint.x, m_cInnerWindowStartPoint.y, cInLocation.x, cInLocation.y);
		}
	}


	if(m_bSelAreaUseCenterMarker)
	{
		HPS::Point center = m_cInnerWindowStartPoint + (cInLocation - m_cInnerWindowStartPoint) * 0.5f;
		m_cSelAreaTempMarkerKey = m_cCrossScratchSegKey.InsertMarker(center);
	}

	GetAttachedView().Update();

	return true;
}

// 2-1. 사각형 Drawing
HPS::LineKey DmiOrbitSelectOperator::InsertRectangle(HPS::SegmentKey & seg, float x0, float y0, float x1, float y1)
{
	HPS::PointArray pts;

	pts.push_back(HPS::Point(x0, y0, 0));
	pts.push_back(HPS::Point(x0, y1, 0));
	pts.push_back(HPS::Point(x1, y1, 0));
	pts.push_back(HPS::Point(x1, y0, 0));
	pts.push_back(HPS::Point(x0, y0, 0));
	return seg.InsertLine(pts);
}

// 2-2. Polygon 사각형 Drawing
HPS::PolygonKey DmiOrbitSelectOperator::InsertPolygon(HPS::SegmentKey & seg, float x0, float y0, float x1, float y1)
{
	HPS::PointArray pts;

	pts.push_back(HPS::Point(x0, y0, 0));
	pts.push_back(HPS::Point(x0, y1, 0));
	pts.push_back(HPS::Point(x1, y1, 0));
	pts.push_back(HPS::Point(x1, y0, 0));
	pts.push_back(HPS::Point(x0, y0, 0));
	return seg.InsertPolygon(pts);
}

// 3. Select Area 처리
bool DmiOrbitSelectOperator::AreaSelectCommon(HPS::WindowKey & cInWindow, HPS::ModifierKeys cInModifiers)
{
	if(true == m_bSelAreaIsRectValid)
	{
		try
		{
			HPS::SelectionResults cNewSelection;
			size_t nSelectedCount = cInWindow.GetSelectionControl().SelectByArea(m_cSelAreaWindowRect, m_cSelAreaSelectionOptions, cNewSelection);

/*
			// ----- Start : Include Select 처리를 위한 Test Code -----
			HPS::SelectionResultsIterator cIterator = cNewSelection.GetIterator();
// 			HPS::Canvas canvas = m_pcHpsView->GetCanvas();
 			HPS::CADModel cCadModel = m_pcHpsView->GetModelHandler()->GetCADModel();

			HPS::VisibilityKit cVisibilityKit;
			m_pcHpsView->GetCanvas().GetFrontView().GetSegmentKey().ShowVisibility(cVisibilityKit);

			while(true == cIterator.IsValid())
			{
				HPS::Key key;
				if(cIterator.GetItem().ShowSelectedItem(key)) {

					HPS::Type cType = key.Type();

					if(key.Type() == HPS::Type::ShellKey)
					{
						HPS::ShellKey shellKey(key);
						// do something with this object
					}
				}

				HPS::KeyPath cSelectKeyPath;
				cIterator.GetItem().ShowPath(cSelectKeyPath);

				HPS::Rectangle cExtentRectangle;
				if(true == cSelectKeyPath.ComputeExtent(cVisibilityKit, cExtentRectangle)) {
					if(false == m_cSelAreaWindowRect.Contains(cExtentRectangle)) {
						int i = 0;
					}
				}

				cIterator.Next();
			}

			for(size_t nIndex = 0; nIndex < nSelectedCount; nIndex++) {

			}

			// ----- End : Include Select 처리를 위한 Test Code -----
*/

			if(m_cActiveSelectionResult.GetCount() > 0 && cInModifiers.Control() && nSelectedCount > 0) {
				m_cActiveSelectionResult.Union(cNewSelection);
			}
			else {
				m_cActiveSelectionResult = cNewSelection;
			}

			if(0 < nSelectedCount)
			{
/*
				auto cIterator = cNewSelection.GetIterator();
				while(true == cIterator.IsValid()) {
					SelectionItem cItem = cIterator.GetItem();

					HPS::Key cKey;
					if(true == cItem.ShowSelectedItem(cKey)) {
						cInWindow.GetHighlightControl().Highlight(cKey, m_cHighlightOptions);
					}

					cIterator.Next();
				}
*/

				cInWindow.GetHighlightControl().Highlight(m_cActiveSelectionResult, m_cHighlightOptions);
				HPS::Database::GetEventDispatcher().InjectEvent(HPS::HighlightEvent(HPS::HighlightEvent::Action::Highlight, m_cActiveSelectionResult, m_cHighlightOptions));
			}
		}
		catch(HPS::InvalidObjectException const &)
		{
			//do nothing
		}
		GetAttachedView().Update();
		return true;
	}
	else
	{
		GetAttachedView().Update();
		return false;
	}
}

//== Highlight 관련 함수 ============================================================================

bool DmiOrbitSelectOperator::HighlightCommon(HPS::WindowKey & cInWindow, HPS::ModifierKeys cInModifiers)
{
	//assert(m_pcHpsView);
	//m_pcHpsView->Unhighlight();

	HPS_UNREFERENCED(cInModifiers);
	HPS::SelectionResults cSelectionResults = GetActiveSelection();
	size_t nSelectedCount = cSelectionResults.GetCount();

	cInWindow.GetHighlightControl().UnhighlightEverything();
	HPS::Database::GetEventDispatcher().InjectEvent(HPS::HighlightEvent(HPS::HighlightEvent::Action::Unhighlight));


	if(0 < nSelectedCount)
	{
		cInWindow.GetHighlightControl().Highlight(cSelectionResults, m_cHighlightOptions);
		HPS::Database::GetEventDispatcher().InjectEvent(HPS::HighlightEvent(HPS::HighlightEvent::Action::Highlight, cSelectionResults, m_cHighlightOptions));
	}

	//m_pcHpsView->CanvasUpdate();

	GetAttachedView().Update();

	return true;
}

bool DmiOrbitSelectOperator::DynamicHighlightCommon(HPS::WindowKey & cInWindow, HPS::ModifierKeys cInModifiers)
{
// 	if(Window::UpdateStatus::Completed != m_cDynamicHighlightUpdateNotifier.Status()) {
// 		return true;
// 	}

	//assert(m_pcHpsView);
	//m_pcHpsView->Unhighlight();

	HPS_UNREFERENCED(cInModifiers);

	ULONG64 nStartTick = GetTickCount64();

	HPS::SelectionResults cOldDynamicActiveSelectionResults = GetOldDynamicActiveSelection();

	if(0 < cOldDynamicActiveSelectionResults.GetCount()) {
		cInWindow.GetHighlightControl().Unhighlight(cOldDynamicActiveSelectionResults);
	}
	
	// 선택된 결과에서 Dynamic Highlight가 작동하더라도 꺼지지 않도록 처리하는 부분
	HPS::SelectionResults cSelectionResults = GetActiveSelection();
	size_t nSelectedCount = cSelectionResults.GetCount();

	if(0 < nSelectedCount)
	{
/*
		auto cIterator = cSelectionResults.GetIterator();
		while(true == cIterator.IsValid()) {
			SelectionItem cItem = cIterator.GetItem();

			HPS::Key cKey;
			if(true == cItem.ShowSelectedItem(cKey)) {
				cInWindow.GetHighlightControl().Highlight(cKey, m_cHighlightOptions);
			}

			cIterator.Next();
		}

		cInWindow.GetHighlightControl().Highlight(cSelectionResults, m_cHighlightOptions);
*/
//		HPS::Database::GetEventDispatcher().InjectEvent(HPS::HighlightEvent(HPS::HighlightEvent::Action::Highlight, cSelectionResults, m_cHighlightOptions));
	}

	nStartTick = GetTickCount64();

	// cInWindow.GetSelectionControl().SelectByPoint(cInLocation, m_cSelectionOptions, cNewSelection) 함수를 통해서 선택됨.
	HPS::SelectionResults cDynamicSelectionResults = GetNewDynamicActiveSelection();
	size_t nDynamicSelectedCount = cDynamicSelectionResults.GetCount();

	if(0 < nDynamicSelectedCount)
	{
		cInWindow.GetHighlightControl().Highlight(cDynamicSelectionResults, m_cDynamicHighlightOptions, false);
/*
		auto cIterator = cDynamicSelectionResults.GetIterator();
		while(true == cIterator.IsValid()) {
			SelectionItem cItem = cIterator.GetItem();

// 			HPS::Key cKey;
// 			if(true == cItem.ShowSelectedItem(cKey)) {
// 				cInWindow.GetHighlightControl().Highlight(cKey, m_cDynamicHighlightOptions);
// 			}

			KeyPath selectionPath;
			cItem.ShowPath(selectionPath);
			cInWindow.GetHighlightControl().Highlight(selectionPath, m_cDynamicHighlightOptions);

			cIterator.Next();
		}
*/

/*
		KeyPath selectionPath;
		SelectionItem selection = it.GetItem();
		selection.ShowPath(selectionPath);

		wk.GetHighlightControl().Highlight(selectionPath, hok);
		it.Next();
*/

		cInWindow.GetHighlightControl().Highlight(cDynamicSelectionResults, m_cDynamicHighlightOptions);
	}
	else {
		return false;
	}

	ULONG64 nEndTick = GetTickCount64();

	TRACE(L"Highlight: %d\r\n", nEndTick - nStartTick);

	//GetAttachedView().Update();

	nStartTick = GetTickCount64();

	//m_pcHpsView->CanvasUpdate();

//  	HPS::UpdateNotifier cUpdateNotifier = GetAttachedView().UpdateWithNotifier(HPS::Window::UpdateType::Refresh);
//  	cUpdateNotifier.Wait();

	//cInWindow.GetSubwindowControl().SetRenderingAlgorithm()
	//GetAttachedView().Update(HPS::Window::UpdateType::Default, 0.05);
// 	HPS::UpdateNotifier cUpdateNotifier = cInWindow.UpdateWithNotifier(HPS::Window::UpdateType::Default, 0.05);
// 	cUpdateNotifier.Wait();
	HPS::UpdateNotifier cUpdateNotifier = cInWindow.UpdateWithNotifier();
	cUpdateNotifier.Wait();

	//cInWindow.Update();

	nEndTick = GetTickCount64();

	TRACE(L"UpdateWithNotifier: %d\r\n", nEndTick - nStartTick);

	return true;
}

//== Orbit 관련 함수 ================================================================================

bool DmiOrbitSelectOperator::OrbitCommon(HPS::WindowPoint const & in_loc)
{
	new_point = in_loc;
	OperatorUtility::ScreenToSphereMousePoint(new_point, new_sphere_pos);

	// get the axis of rotation
	HPS::Vector axis = start_sphere_pos.Cross(new_sphere_pos);

	// this is for screen mouse based movement
	HPS::Vector vtmp = new_point - start_point;
	float dist = static_cast<float>(vtmp.Length()) * 90.0f;

	if((axis.x != 0.0f || axis.y != 0.0f || axis.z != 0.0f))
	{
		View view = GetAttachedView();
		SegmentKey key = view.GetSegmentKey();
		HPS::Drawing::Handedness worldHandedness;

		if(key.GetDrawingAttributeControl().ShowWorldHandedness(worldHandedness)
			&& worldHandedness == HPS::Drawing::Handedness::Right)
		{
			axis.y *= -1;
			axis.z *= -1;
		}

		axis.Normalize();

		float	theta = OperatorUtility::CalculateAngleOrbitOnPlane(axis, OperatorUtility::Plane_YZ);
		float	angle2 = axis.x < 0 ? -theta * dist : theta * dist;

		theta = OperatorUtility::CalculateAngleOrbitOnPlane(axis, OperatorUtility::Plane_XZ);
		float	angle1 = axis.y < 0 ? theta * dist : -theta * dist;

		theta = OperatorUtility::CalculateAngleOrbitOnPlane(axis, OperatorUtility::Plane_XY);
		float	angle3 = axis.z < 0 ? theta * dist : -theta * dist;

		HPS::CameraKit cam;
		key.ShowCamera(cam);
		cam.Orbit(angle1, angle2);
		cam.Roll(angle3);
		key.SetCamera(cam);
		Database::GetEventDispatcher().InjectEvent(CameraChangedEvent(GetAttachedView()));
	}

	start_sphere_pos = new_sphere_pos;
	start_point = new_point;

	return true;
}

//== Select 관련 함수 ================================================================================

// 1. 선택된 항목 처리
void DmiOrbitSelectOperator::SelectedItemProcessing()
{
	HPS::SelectionResults cSelectionResults = GetActiveSelection();
	size_t nSelectedCount = cSelectionResults.GetCount();

	CString strMessage;

	if(0 < nSelectedCount) {
		SelectionResultsIterator cIterator = cSelectionResults.GetIterator();

		while(true == cIterator.IsValid())
		{
			SelectionItem cSelectedItem = cIterator.GetItem();

			Key cSelItemkey;
			cSelectedItem.ShowSelectedItem(cSelItemkey);

/*
			if(HPS::Type::ShellKey == cSelItemkey.Type()) {
				HPS::ShellKey cShellKey = HPS::ShellKey(cSelItemkey);
				if(0 < cShellKey.ShowUserDataCount()) {
					ByteArray aByteArray;
					cShellKey.ShowUserData(0, aByteArray);

					CString strShellName;
					DmiHps::CharToCString((char *) aByteArray.data(), strShellName);

					if(false == strMessage.IsEmpty()) {
						strMessage += L", ";
					}

					strMessage += strShellName;
				}
			}
*/

			m_cCurrentSelectKey = cSelItemkey;

			cIterator.Next();
		}
	}

	if(false == strMessage.IsEmpty()) {
		// #Require_convert_new_version
/*
		Json::Object cCommand;
		cCommand.SetString("Type", MSG_APP_TYPE);
		cCommand.SetInteger("MSG", MSG_SELECTED_ITEM_INFORMATION);
		cCommand.SetString("ItemInfo", strMessage);

		m_pcHpsView->SendExecuteCommand(cCommand);*/
	}
}
