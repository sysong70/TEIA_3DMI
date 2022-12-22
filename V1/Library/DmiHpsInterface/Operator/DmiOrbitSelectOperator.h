#pragma once

#include "DmiSelectOperator.h"

class DmiHpsView;

class DmiOrbitSelectOperator : public DmiSelectOperator
{
public:
	// Constructor for DmiHighlightOperator
	// param: cInMouseTrigger The mouse button that will activate this operator
	// param: cInModifierTrigger The modifier button that, when pressed with the mouse trigger, will activate this operator
	DmiOrbitSelectOperator(DmiHpsView * pcHpsView, HPS::MouseButtons cInMouseTrigger = HPS::MouseButtons::ButtonLeft(), 
		HPS::ModifierKeys cInModifierTrigger = HPS::ModifierKeys());

	HPS::UTF8 GetName() const override { return "DMI_OrbitSelectOperator"; }

	void OnViewAttached(HPS::View const & cInAttachedView) override;
	void SetupConstructionSegment();
	void SetupPortfolio(HPS::View const & cInAttachedView);

	bool OnMouseDown(HPS::MouseState const & in_state) override;
	bool OnMouseUp(HPS::MouseState const & in_state) override;
	bool OnMouseMove(HPS::MouseState const & cInState) override;

	bool OnTouchDown(HPS::TouchState const & in_state) override;

	// Accepts a HighlightOptionsKit that defines how an object will be highlighted.
	// param: cInOptions The HighlightOptionsKit from which the highlight options will be set

	void SetHighlightOptions(HPS::HighlightOptionsKit const & cInOptions) { m_cHighlightOptions = cInOptions; }

	// Gets the HPS::HighlightOptionsKit associated with this operator.
	// return: The HPS::HighlightOptionsKit associated with this operator
	HPS::HighlightOptionsKit GetHighlightOptions() const { return m_cHighlightOptions; }

private:
	bool HighlightCommon(HPS::WindowKey & in_window, HPS::ModifierKeys in_modifiers);
	bool DynamicHighlightCommon(HPS::WindowKey & in_window, HPS::ModifierKeys in_modifiers);

	HPS::HighlightOptionsKit m_cHighlightOptions;
	HPS::HighlightOptionsKit m_cDynamicHighlightOptions;
	HPS::UpdateNotifier m_cDynamicHighlightUpdateNotifier;

	//== Orbit Operator 관련 ========================================================================
private:
	bool OrbitCommon(HPS::WindowPoint const & cInLocation);

	//== Select 관련 함수 ============================================================================
protected:
	void SelectedItemProcessing();

	bool m_bOrbitMode = false;
	bool m_bOperatorActiveFlag;
	HPS::TouchID tracked_touch_ID;
	HPS::WindowPoint start_point, new_point;
	HPS::Vector start_sphere_pos, new_sphere_pos;

	HPS::PixelPoint m_cMouseDownPoint;
	DWORD m_nMouseDownTickCount;
	DWORD m_nSelectPickCount;

	DmiHpsView * m_pcHpsView = nullptr;

	HPS::Key m_cCurrentSelectKey;

	//== Area Select Function =======================================================================
protected:
	bool OnAreaSelectMouseUp(HPS::MouseState const & cInState);

	bool ConstructRectCommon(HPS::WindowPoint const & cInLocation);
	HPS::LineKey InsertRectangle(HPS::SegmentKey & seg, float x0, float y0, float x1, float y1);
	HPS::PolygonKey InsertPolygon(HPS::SegmentKey & seg, float x0, float y0, float x1, float y1);

	bool AreaSelectCommon(HPS::WindowKey & cInWindow, HPS::ModifierKeys cInModifiers);

	// ----- Area Select 관련 변수 -----
	HPS::SegmentKey m_cCrossScratchSegKey;
	HPS::SegmentKey m_cIncludeScratchSegKey;

	HPS::WindowPoint m_cWindowStartPoint;
	HPS::WindowPoint m_cInnerWindowStartPoint;

	HPS::Rectangle m_cSelAreaWindowRect;
	HPS::Rectangle m_cSelAreaInnerWindowRect;

	HPS::LineKey m_cSelAreaTempLineKey;
	HPS::PolygonKey m_cSelAreaTempPolygonKey;
	HPS::MarkerKey m_cSelAreaTempMarkerKey;

	HPS::SelectionOptionsKit m_cSelAreaSelectionOptions;


	bool m_bSelAreaUseCenterMarker = false;
	bool m_bDrawFaces = true;
	bool m_bAreaSelectMode = false;
	bool m_bSelAreaIsRectValid = false;
};