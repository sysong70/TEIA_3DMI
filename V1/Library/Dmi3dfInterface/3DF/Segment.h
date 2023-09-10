#pragma once

#include "Key.h"

#include "Include.h"
#include "Shell.h"
#include "3DF.EdgeAttribute.h"
//#include "3DF.Portfolio.h"
#include "Style.h"
#include "3DF.Search.h"

#include "Marker.h"

#include "Math.Matrix.h"

OPEN_3DF_NAMESPACE

class API_3DF SegmentKey : public Key
{
public:
	SegmentKey(CString strInName);
	SegmentKey(HC_KEY nInKey = INVALID_KEY);
	SegmentKey(SegmentKey const & cInThat);

	virtual ~SegmentKey();

	void Set(SegmentKey const & cInThat);
	SegmentKey & operator = (SegmentKey const & cInThat);

	//== Segment 관련 함수 ===========================================================================
	SegmentKey & Open();
	SegmentKey & Close();

	// Segment의 자체의 Title을 변경함.
	CString Name() const;
	SegmentKey & SetName(CString strInName);

	//== Sub Segment 관련 함수 =======================================================================
	SegmentKey const Subsegment();
	SegmentKey const Subsegment(LPCTSTR pszFromat, ...);
	
	size_t ShowSubsegments() const;
	size_t ShowSubsegments(SegmentKeyArray & cOutChildren) const;

	//== Flush 관련 함수 =============================================================================
	void Flush(Search::Type eInTypeToRemove = Search::Type::Everything, Search::Space eInSearchSpace = Search::Space::SegmentOnly);
	void Flush(SearchTypeArray const & aInTypesToRemove, Search::Space eInSearchSpace = Search::Space::SegmentOnly);
	void Flush(size_t nInTypesCount, Search::Type const peInTypesToRemove[], Search::Space eInSearchSpace = Search::Space::SegmentOnly);

	//== Include 관련 함수 ===========================================================================
	IncludeKey IncludeSegment(SegmentKey const & cInSegment);

	size_t ShowIncluders(SegmentKeyArray & aOutSegments) const;
	size_t ShowIncluders(IncludeKeyArray & aOutIncludes) const;

	//== Shell 관련 함수 =============================================================================
	ShellKey InsertShell(ShellKit const & cInKit);
// 	ShellKey InsertShell(PointArray const & in_points, IntArray const & in_facelist);
// 	ShellKey InsertShell(size_t in_point_count, Point const in_points[], size_t in_facelist_count, int const in_facelist[]);

	//== Edge 관련 함수 ==============================================================================
	EdgeAttributeControl GetEdgeAttributeControl();

	//== Line 관련 함수 ==============================================================================
	LineKey InsertLine(size_t nInCount, Point const pcInPoints[]);

	LineAttributeControl GetLineAttributeControl();
	LineAttributeControl const GetLineAttributeControl() const;

	//== Circle 관련 함수 ============================================================================
	CircleKey InsertCircle(Point const & cInCenter, float fInRadius, Vector const & cInNormal);

	//== Marker 관련 함수 ============================================================================
	MarkerKey InsertMarker(Point const & in_position);
	MarkerKey InsertMarker(double x, double y, double z);

	MarkerAttributeControl GetMarkerAttributeControl();
	MarkerAttributeControl const GetMarkerAttributeControl() const;

	//== Material Mapping 관련 함수 ==================================================================
	SegmentKey & SetMaterialMapping(MaterialMappingKit const & cInKit);
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

	//== Visual Effect Control 관련 함수 =============================================================
	VisualEffectsControl GetVisualEffectsControl();
	VisualEffectsControl const GetVisualEffectsControl() const;

	//== Condition 관련 함수 =========================================================================
	SegmentKey & SetCondition(CString strInCondition);
	
	//== Heuristics 관련 함수 ========================================================================
	SegmentKey & SetHeuristics(CString strInHeuristics);

	//== Portfolio Control 관련 함수 =================================================================
	//PortfolioControl GetPortfolioControl();
	//PortfolioControl const GetPortfolioControl() const;

	//== Style Control 관련 함수 =====================================================================
	StyleControl GetStyleControl();

	void SetRenderingOptions(CString strList);
	void SetColorByIndex(CString strList, int nIndex);
	void SetMarkerSymbol(CString strSymbol);		

	//== Camera 관련 함수 ===========================================================================
// 	SegmentKey & SetCamera(CameraKit const & cInKit);
// 	SegmentKey & UnsetCamera();
	bool ShowCamera(CameraKit & cOutKit) const;

	//== Modelling Matrix 관련 함수 ==================================================================
	SegmentKey & SetModellingMatrix(MatrixKit const & cInKit);
	SegmentKey & UnsetModellingMatrix();
	bool ShowModellingMatrix(MatrixKit & cOutKit) const;

	//== Bounding 관련 함수 ==========================================================================
	SegmentKey & SetBounding(BoundingKit const & cInKit);

	//== User Data 관련 함수 =========================================================================
	SegmentKey & SetUserData(IntPtrTArray const & aInIndices, ByteArrayArray const & aInData);
	SegmentKey & SetUserData(intptr_t nInIndex, size_t nInBytes, BYTE const pnInData[]);
	SegmentKey & SetUserData(intptr_t nInIndex, ByteArray const & aInData);

	SegmentKey & UnsetUserData(intptr_t nInIndex);
	SegmentKey & UnsetUserData(size_t nInCount, intptr_t const pnInIndices[]);
	SegmentKey & UnsetUserData(IntPtrTArray const & pnInIndices);
	SegmentKey & UnsetAllUserData();

	size_t ShowUserDataCount() const;
	bool ShowUserData(IntPtrTArray & aOutIndices, ByteArrayArray & aOutData) const;
	bool ShowUserDataIndices(IntPtrTArray & aOutIndices) const;
	bool ShowUserData(intptr_t nInIndex, ByteArray & aOutData) const;
};

CLOSE_3DF_NAMESPACE