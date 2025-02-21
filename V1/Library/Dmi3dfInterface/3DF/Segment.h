#pragma once

#include "Key.h"

#include "Include.h"
#include "Shell.h"
#include "EdgeAttribute.h"
#include "Style.h"
#include "Search.h"

#include "Marker.h"

#include "Math.Matrix.h"

namespace H3DF
{
	class API_3DF SegmentKey : public Key
	{
	public:
		SegmentKey();	
		SegmentKey(CStringA strInName);
		SegmentKey(HC_KEY nInKey);
		SegmentKey(SegmentKey const & cInThat);

		virtual ~SegmentKey();

		H3DF::Type ObjectType() const { return H3DF::Type::SegmentKey; };

		void Set(SegmentKey const & cInThat);
		SegmentKey & operator = (SegmentKey const & cInThat);

		//== Segment 관련 함수 =======================================================================
		SegmentKey & Open();
		SegmentKey & Close();

		// Segment의 자체의 Title을 변경함.
		CStringA Name(bool bIncludePath = true) const;
		SegmentKey & SetName(CStringA strInName);

		//== Sub Segment 관련 함수 ===================================================================
		SegmentKey const Subsegment();
		SegmentKey const Subsegment(LPCSTR pszFromat, ...);

		size_t ShowSubsegments() const;
		size_t ShowSubsegments(SegmentKeyArray & cOutChildren) const;

		//== Flush 관련 함수 =========================================================================
		void Flush(Search::Type eInTypeToRemove = Search::Type::Everything, Search::Space eInSearchSpace = Search::Space::SegmentOnly);
		void Flush(SearchTypeArray const & aInTypesToRemove, Search::Space eInSearchSpace = Search::Space::SegmentOnly);
		void Flush(size_t nInTypesCount, Search::Type const peInTypesToRemove[], Search::Space eInSearchSpace = Search::Space::SegmentOnly);

		//== Find 관련 함수 ==========================================================================
		size_t Find(Search::Type eInRequest, Search::Space eInSearchSpace, SearchResults & cOutResults) const;

		//== Include 관련 함수 =======================================================================
		IncludeKey IncludeSegment(SegmentKey const & cInSegment);
		IncludeKey IncludeSegment(SegmentKey const & cInSegment, ConditionalExpression const & cInConditional);

		size_t ShowIncluders() const;
		size_t ShowIncluders(SegmentKeyArray & aOutSegments) const;
		size_t ShowIncluders(IncludeKeyArray & aOutIncludes) const;

		//== Reference 관련 함수 =====================================================================
		ReferenceKey ReferenceGeometry(Key const & cInKey);
		//size_t ShowReferrers(SegmentKeyArray & aOutSegments) const;
		size_t ShowReferrers(ReferenceKeyArray & aOutReferences) const;

		//== Cutting Section 관련 함수 ===============================================================
 		CuttingSectionKey InsertCuttingSection(Plane const & cInPlane);
// 		CuttingSectionKey InsertCuttingSection(CuttingSectionKit const & cInKit);
// 		CuttingSectionKey InsertCuttingSection(PlaneArray const & cInPlanes);
// 		CuttingSectionKey InsertCuttingSection(size_t nInPlanesCount, Plane const pcInPlanes[]);

		//== Shell 관련 함수 =========================================================================
		ShellKey InsertShell(ShellKit const & cInKit);
		// 	ShellKey InsertShell(PointArray const & in_points, IntArray const & in_facelist);
		// 	ShellKey InsertShell(size_t in_point_count, Point const in_points[], size_t in_facelist_count, int const in_facelist[]);

		//== Edge 관련 함수 ==========================================================================
		EdgeAttributeControl GetEdgeAttributeControl();

		//== Line 관련 함수 ==========================================================================
		LineKey InsertLine(size_t nInCount, Point const pcInPoints[]);

		LineAttributeControl GetLineAttributeControl();
		LineAttributeControl const GetLineAttributeControl() const;

		//== Circle 관련 함수 ========================================================================
		CircleKey InsertCircle(Point const & cInCenter, float fInRadius, Vector const & cInNormal);

		//== Marker 관련 함수 ========================================================================
		MarkerKey InsertMarker(Point const & in_position);
		MarkerKey InsertMarker(double x, double y, double z);

		MarkerAttributeControl GetMarkerAttributeControl();
		MarkerAttributeControl const GetMarkerAttributeControl() const;

		//== Material Mapping 관련 함수 ==============================================================
		SegmentKey & SetMaterialMapping(MaterialMappingKit const & cInKit);
		MaterialMappingControl GetMaterialMappingControl();
		MaterialMappingControl const GetMaterialMappingControl() const;

		//== Performance Control 관련 함수 ===========================================================
		SegmentKey & SetPerformance(PerformanceKit const & cInKit);
		SegmentKey & UnsetPerformance();
		bool ShowPerformance(PerformanceKit & cOutKit) const;
		PerformanceControl GetPerformanceControl();
		PerformanceControl const GetPerformanceControl() const;

		//== Selectability Control 관련 함수 =========================================================
		SelectabilityControl GetSelectabilityControl();
		SelectabilityControl const GetSelectabilityControl() const;

		//== Visibility Control 관련 함수 ============================================================
		VisibilityControl GetVisibilityControl();
		VisibilityControl const GetVisibilityControl() const;
		SegmentKey & SetVisibility(CString strList);

		//== Visual Effect Control 관련 함수 =========================================================
		VisualEffectsControl GetVisualEffectsControl();
		VisualEffectsControl const GetVisualEffectsControl() const;

		//== Heuristics 관련 함수 ====================================================================
		SegmentKey & SetHeuristics(CString strInHeuristics);

		//== Portfolio Control 관련 함수 =============================================================
		PortfolioControl GetPortfolioControl();
		PortfolioControl const GetPortfolioControl() const;

		//== Style Control 관련 함수 =================================================================
		StyleControl GetStyleControl();

		//== Condition 관련 함수 =====================================================================
		SegmentKey & SetCondition(CStringA strInCondition);
		SegmentKey & SetConditions(AStringArray const & astrInConditions);
		SegmentKey & UnsetConditions();

		ConditionControl GetConditionControl();
		ConditionControl const GetConditionControl() const;

		void SetRenderingOptions(CString strList);
		void SetColorByIndex(CString strList, int nIndex);
		void SetMarkerSymbol(CString strSymbol);

		//== Camera 관련 함수 ========================================================================
	 	SegmentKey & SetCamera(CameraKit const & cInKit);
		SegmentKey & SetCamera(MatrixKit & cInKit);

	 	//SegmentKey & UnsetCamera();
		bool ShowCamera(CameraKit & cOutKit) const;

		CameraControl GetCameraControl();
		CameraControl const GetCameraControl() const;

		//== Modelling Matrix 관련 함수 ==============================================================
		SegmentKey & SetModellingMatrix(MatrixKit const & cInKit);
		SegmentKey & UnsetModellingMatrix();
		bool ShowModellingMatrix(MatrixKit & cOutKit) const;

		//== Bounding 관련 함수 ======================================================================
		SegmentKey & SetBounding(BoundingKit const & cInKit);
		SegmentKey & UnsetBounding();
		bool ShowBounding(BoundingKit & cOutkit) const;

 		BoundingControl GetBoundingControl();
 		BoundingControl const GetBoundingControl() const;

		//== Attribute Lock 관련 함수 ================================================================
// 		SegmentKey & SetAttributeLock(AttributeLockKit const & in_kit);
// 		SegmentKey & UnsetAttributeLock();
// 		bool ShowAttributeLock(AttributeLockKit & out_kit) const;
		AttributeLockControl GetAttributeLockControl();
		AttributeLockControl const GetAttributeLockControl() const;

		//== Drawing Attribute 관련 함수 =============================================================
 		SegmentKey & SetDrawingAttribute(DrawingAttributeKit const & cInKit);
// 		SegmentKey & UnsetDrawingAttribute();
// 		bool ShowDrawingAttribute(DrawingAttributeKit & out_kit) const;
		DrawingAttributeControl GetDrawingAttributeControl();
		DrawingAttributeControl const GetDrawingAttributeControl() const;

		//== Color Interpolation 관련 함수 ==========================================================
		ColorInterpolationControl GetColorInterpolationControl();
		ColorInterpolationControl const GetColorInterpolationControl() const;

		//== Culling 관련 함수 =======================================================================
		CullingControl GetCullingControl();
		CullingControl const GetCullingControl() const;

		//== User Data 관련 함수 =====================================================================
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

		//== Information 관련 함수 ===================================================================
		bool ShowRenderingOptions(CStringA & strList) const;
		bool ShowVisibility(CStringA & strList) const;
		bool ShowSelectability(CStringA & strList) const;
		bool ShowHeuristics(CStringA & strList) const;
		bool ShowDriverOptions(CStringA & strList) const;
	};
}