#pragma once

#include "ConvObject.h"

#include <curve.h>
#include <curve3d.h>

class MbSurface;

class ConvCurve : public ConvObject
{
public:
	ConvCurve(A3DCrvBase * pcUVCurve, double dContextScale);
	~ConvCurve();

	bool ConvertPlaneCurve(const MbSurface * pcSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);

private:
	ConvEntityInfo m_cEntityInfo;
};

namespace ConvCurveBase
{
	// == Curve 정보 수집 ============================================================================
	bool GetCurveData(const A3DCrvBase * pcCrvBase, double dContextScale, ConvEntityInfo & cEntityInfo);
	bool ClearCurveData(A3DEEntityType & eCurveType, DWORD_PTR *& pcCurveData,
		A3DEEntityType & eRelationEntityType, DWORD_PTR *& pcRelationEntityData);

	// == Plane Curve 관련 함수 ==========================================================================
	bool ConvertPlaneCurve(const A3DCrvBase * pcCrvBase, const MbSurface * pcSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);
	bool ConvertPlaneCurveData(DWORD_PTR * pcCurveData, A3DEEntityType & eEntityType, const MbSurface * pcBaseSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);

	bool ConvertPlaneLine(const A3DCrvBase * pcCrvBase, const MbSurface * pcSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);
	bool ConvertPlaneCircle(const A3DCrvBase * pcCrvBase, const MbSurface * pcSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);

	bool ConvertPlaneNurbs(const A3DCrvBase * pcCrvBase, const MbSurface * pcSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);
	bool ConvertPlaneNurbsData(DWORD_PTR * pcCurveData, const MbSurface * pcBaseSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);
	bool ConvertPlaneNurbsData(ConvEntityInfo & cEntityInfo, const MbSurface * pcBaseSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);

	bool ConvertProjectionPlaneCurve(const A3DCrvBase * pcCrvBase, const MbSurface * pcSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);
	bool ConvertProjectionPlaneCurveData(DWORD_PTR * pcCurveData, A3DEEntityType & eEntityType, const MbSurface * pcBaseSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve);

	// == Space Curve 관련 함수 ======================================================================
	bool ConvertSpaceCurve(const A3DCrvBase * pcCrvBase, double dParentScale, double dContextScale, c3d::SpaceCurveSPtr & pcSpaceCurve);
	bool ConvertSpaceCurveData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, c3d::SpaceCurveSPtr & pcSpaceCurve);
	bool ConvertSpaceCurveData(A3DTopoWireEdgeData & cWireEdgeData, double dParentScale, double dContextScale, c3d::SpaceCurveSPtr & pcSpaceCurve);

	bool ConvertSpaceLine(const A3DCrvBase * pc3dCurve, double dParentScale, c3d::SpaceCurveSPtr & pcSpaceCurve);
	//bool ConvertSpaceLineData(ConvEntityInfo & cEntityInfo, double dParentScale, c3d::SpaceCurveSPtr & pcSpaceCurve);

	bool ConvertSpacePolyLine(const A3DCrvBase * pc3dCurve, double dParentScale, c3d::SpaceCurveSPtr & pcSpaceCurve);

	bool ConvertSpaceCircle(const A3DCrvBase * pc3dCurve, double dParentScale, c3d::SpaceCurveSPtr & pcSpaceCurve);

	bool ConvertSpaceEllipse(const A3DCrvBase * pc3dCurve, double dParentScale, c3d::SpaceCurveSPtr & pcSpaceCurve);

	bool ConvertSpaceNurbs(const A3DCrvBase * pc3dCurve, double dParentScale, double dContextScale, c3d::SpaceCurveSPtr & pcSpaceCurve);
	bool ConvertSpaceNurbsData(const A3DCrvNurbsData & cCrvNurbsData, double dParentScale, double dContextScale, c3d::SpaceCurveSPtr & pcSpaceCurve);

	bool ConvertSpaceCompositeCurve(const A3DCrvBase * pc3dCurve, double dParentScale, double dContxtScale, c3d::SpaceCurveSPtr & pcSpaceCurve);

	bool ConvertSpaceGetAsNurbsCurve(const A3DCrvBase * pc3dCurve, double dParentScale, double dContextScale, c3d::SpaceCurveSPtr & pcSpaceCurve);

	// == Utility ==================================================================================
	void CalculateSurfaceCoeffPoint(const MbSurface * pcSurface, MbCartPoint & cPoint);

	bool GetTargetEntityIndex(ConvEntityInfo & cEntityInfo, int & nTargetIndex);

	bool GetCurveCoeffcient(ConvEntityInfo & cEntityInfo, double & dCoeffA, double & dCoeffB);
};