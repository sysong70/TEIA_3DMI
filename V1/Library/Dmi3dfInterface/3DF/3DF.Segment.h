#pragma once

#include "3DF.Key.h"

#include "3DF.Include.h"
#include "3DF.Shell.h"
#include "3DF.Portfolio.h"
#include "3DF.Style.h"

#include "3DF.Marker.h"

OPEN_3DF_NAMESPACE

class API_3DF SegmentKey : public Key
{
public:
	SegmentKey();
	SegmentKey(CString strInName);
	SegmentKey(HC_KEY nKey);
	SegmentKey(SegmentKey const & cInThat);

	virtual ~SegmentKey();

	SegmentKey & operator = (SegmentKey const & cOther);

	SegmentKey Subsegment() const;
	SegmentKey Subsegment(LPCTSTR pszFromat, ...) const;

	CString Name() const;
	SegmentKey & SetName(CString strInName);

	//== Include 관련 함수 ===========================================================================
	IncludeKey IncludeSegment(SegmentKey const & cInSegment);

	//== Shell 관련 함수 =============================================================================
	ShellKey InsertShell(ShellKit const & cInKit);
// 	ShellKey InsertShell(PointArray const & in_points, IntArray const & in_facelist);
// 	ShellKey InsertShell(size_t in_point_count, Point const in_points[], size_t in_facelist_count, int const in_facelist[]);

	//== Line 관련 함수 ==============================================================================
	LineKey InsertLine(size_t nInCount, Point const pcInPoints[]);

	//== Marker 관련 함수 ============================================================================
	MarkerKey InsertMarker(Point const & in_position);
	MarkerKey InsertMarker(double x, double y, double z);

	MarkerAttributeControl GetMarkerAttributeControl();
	MarkerAttributeControl const GetMarkerAttributeControl() const;

	//== Material Mapping 관련 함수 ==================================================================
	SegmentKey & SetMaterialMapping(CString strGeometry, MaterialMappingKit const & cInKit);
	MaterialMappingControl GetMaterialMappingControl();
	MaterialMappingControl const GetMaterialMappingControl() const;

	//== Select Control 관련 함수 ====================================================================
	SelectabilityControl GetSelectabilityControl();
	SelectabilityControl const GetSelectabilityControl() const;
	SegmentKey & SetSelectability(CString strText);

	//== Visibility Control 관련 함수 ================================================================
	VisibilityControl GetVisibilityControl();
	VisibilityControl const GetVisibilityControl() const;
	SegmentKey & SetVisibility(CString strList);

	//== Condition 관련 함수 =========================================================================
	SegmentKey & SetCondition(CString strInCondition);
	
	//== Portfolio Control 관련 함수 =================================================================
	//PortfolioControl GetPortfolioControl();
	//PortfolioControl const GetPortfolioControl() const;

	//== Style Control 관련 함수 =====================================================================
	StyleControl GetStyleControl();

	void SetRenderingOptions(CString strList);
	void SetColorByIndex(CString strList, int nIndex);
	void SetMarkerSymbol(CString strSymbol);

	//== Model Segment 관련 함수 =====================================================================
	void ConfigureSegmentModel();
	SegmentKey ModelInclude();
	SegmentKey ModelInclude() const;
	SegmentKey StylesInclude();
	SegmentKey StylesInclude() const;

	SegmentKey & SetModellingMatrix(MatrixKit const & cInKit);

	//== Bounding 관련 함수 ==========================================================================
	SegmentKey & SetBounding(BoundingKit const & cInKit);

private:
	//bool m_bOpenSegment = false; // 현재 Segment가 Open되어 있는지를 나타내는 flag

	HC_KEY m_nModelIncludeKey = INVALID_KEY;
	HC_KEY m_nStylesIncludeKey = INVALID_KEY;
};

CLOSE_3DF_NAMESPACE