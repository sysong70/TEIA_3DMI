#include "stdafx.h"

#include "ConvCurve.h"
#include "ConvSurface.h"

#include "../Dmi3dx.h"

#include <surface.h>
#include <surf_curve_bounded_surface.h>
#include <surf_plane.h>

// 3D curve
#include <curve3d.h>
#include <cur_line_segment3d.h>
#include <cur_line3d.h>
#include <cur_arc3d.h>
#include <cur_nurbs3d.h>
#include <cur_surface_intersection.h>
#include <cur_surface_curve.h>
#include <cur_contour3d.h>
#include <cur_polyline3d.h>

// 2D curve
#include <cur_line_segment.h>
#include <cur_nurbs.h>
#include <cur_reparam_curve.h>
#include <cur_projection_curve.h>

#include <action_curve.h>

#include "../Surface/DmiSurface.h"
#include "../Surface/DmiCylinderSurface.h"
#include "../Surface/DmiConeSurface.h"
#include "../Surface/DmiSphereSurface.h"
#include "../Surface/DmiTorusSurface.h"

#include "../C3dTracer.h"

namespace ConvCurveBase
{
	double dTolerance = 1.0e-6;
};

using namespace c3d;

ConvCurve::ConvCurve(A3DCrvBase * pcUVCurve, double dContextScale)
{
	// ----- Curve 정보 수집 -----
	if(false == ConvCurveBase::GetCurveData(pcUVCurve, dContextScale, m_cEntityInfo)) {
		return;
	}

	m_bInitFlag = true;
}

ConvCurve::~ConvCurve()
{
	if(false == m_bInitFlag) {
		return;
	}

	// #temp : ConvCurveBase::ClearCurveData
	//ConvCurveBase::ClearCurveData(m_cEntityInfo);
}

bool ConvCurve::ConvertPlaneCurve(const MbSurface * pcBaseSurface, double dParentScale, PlaneCurveSPtr & pcPlaneCurve)
{
	int nTargetIndxe = -1;
	if(false == ConvCurveBase::GetTargetEntityIndex(m_cEntityInfo, nTargetIndxe)) {
		return false;
	}

	A3DEEntityType eCurveType = m_cEntityInfo.peEntityType[nTargetIndxe];

	bool bStatus = false;
	switch(eCurveType)
	{

		/*
				case kA3DTypeCrvLine:
					bStatus = ConvertPlaneLine(pcSurface, dTopoContextScale, pcPlaneCurve);
					break;

				case kA3DTypeCrvCircle:
					bStatus = ConvertPlaneCircle(pcSurface, dTopoContextScale, pcPlaneCurve);
					break;
		*/
		case kA3DTypeCrvNurbs:
			bStatus = ConvCurveBase::ConvertPlaneNurbsData(m_cEntityInfo, pcBaseSurface, dParentScale, pcPlaneCurve);
			break;
	}

	if(false == bStatus) {
		ASSERT(false);
		CStringA strType = Dmi3dx::GetA3dEntityTypeString(eCurveType);
		LogManager::Log(2, "ConvertPlaneCurve Error: %s", strType);
		return false;
	}

	return bStatus;
}



// == 1. Curve 정보 수집 =============================================================================

// 1. Curve 정보 수집
bool ConvCurveBase::GetCurveData(const A3DCrvBase * pcCrvBase, double dContextScale, ConvEntityInfo & cEntityInfo)
{
	A3DEEntityType eCurveType = kA3DTypeUnknown;
	CHECK_A3D_RESULT(A3DEntityGetType(pcCrvBase, &eCurveType));

	double dTolerance = ConvCurveBase::dTolerance / dContextScale;

	int nIndex = cEntityInfo.nTargetEntityIndex;
	if(-1 == nIndex) {
		// Entity count는 한개임.
		int nEntityCount = 1;
		cEntityInfo.Init(nEntityCount);
		nIndex = cEntityInfo.nTargetEntityIndex = 0;
	}

	cEntityInfo.peEntityType[nIndex] = eCurveType;

	A3DStatus nStatus = A3D_ERROR;
	DWORD_PTR * pcData = nullptr;
	switch(eCurveType)
	{
		case kA3DTypeCrvLine:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DCrvLineData();
			A3D_INITIALIZE_DATA(A3DCrvLineData, (*((A3DCrvLineData *) cEntityInfo.pcEntityData[nIndex])));
			nStatus = A3DCrvLineGet(pcCrvBase, (A3DCrvLineData *) cEntityInfo.pcEntityData[nIndex]);
			break;

		case kA3DTypeCrvCircle:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DCrvCircleData();
			A3D_INITIALIZE_DATA(A3DCrvCircleData, (*((A3DCrvCircleData *) cEntityInfo.pcEntityData[nIndex])));
			nStatus = A3DCrvCircleGet(pcCrvBase, (A3DCrvCircleData *) cEntityInfo.pcEntityData[nIndex]);
			break;

		case kA3DTypeCrvEllipse:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DCrvEllipseData();
			A3D_INITIALIZE_DATA(A3DCrvEllipseData, (*((A3DCrvEllipseData *) cEntityInfo.pcEntityData[nIndex])));
			nStatus = A3DCrvEllipseGet(pcCrvBase, (A3DCrvEllipseData *) cEntityInfo.pcEntityData[nIndex]);
			break;

		case kA3DTypeCrvNurbs:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DCrvNurbsData();
			A3D_INITIALIZE_DATA(A3DCrvNurbsData, (*((A3DCrvNurbsData *) cEntityInfo.pcEntityData[nIndex])));
			nStatus = A3DCrvNurbsGet(pcCrvBase, (A3DCrvNurbsData *) cEntityInfo.pcEntityData[nIndex]);
			break;

		case kA3DTypeCrvComposite:
		case kA3DTypeCrvOnSurf:
		case kA3DTypeCrvOffset:
			pcData = cEntityInfo.pcEntityData[nIndex] = (DWORD_PTR *)new A3DCrvNurbsData();
			A3D_INITIALIZE_DATA(A3DCrvNurbsData, (*((A3DCrvNurbsData *) cEntityInfo.pcEntityData[nIndex])));
			nStatus = A3DCrvBaseGetAsNurbs(pcCrvBase, dTolerance, true, (A3DCrvNurbsData *) cEntityInfo.pcEntityData[nIndex]);
			eCurveType = kA3DTypeCrvNurbs;
			cEntityInfo.peEntityType[nIndex] = eCurveType;
			break;
	}

	if(A3D_SUCCESS != nStatus) {
		ASSERT(false);
		CStringA strType = Dmi3dx::GetA3dEntityTypeString(eCurveType);
		LogManager::Log(2, "GetCurveData Error: %s", strType);
		return false;
	}

	return true;
}

bool ConvCurveBase::ClearCurveData(A3DEEntityType & eCurveType, DWORD_PTR *& pcCurveData,
	A3DEEntityType & eRelationEntityType, DWORD_PTR *& pcRelationEntityData)
{
	switch(eCurveType)
	{
		case kA3DTypeCrvLine:
			A3DCrvLineGet(nullptr, (A3DCrvLineData *) pcCurveData);
			break;

		case kA3DTypeCrvCircle:
			A3DCrvCircleGet(nullptr, (A3DCrvCircleData *) pcCurveData);
			break;

		case kA3DTypeCrvEllipse:
			A3DCrvEllipseGet(nullptr, (A3DCrvEllipseData *) pcCurveData);
			break;

		case kA3DTypeCrvNurbs:
			A3DCrvNurbsGet(nullptr, (A3DCrvNurbsData *) pcCurveData);
			break;

		case kA3DTypeCrvComposite:
			ASSERT(false);
			break;

		case kA3DTypeCrvOnSurf:
			ASSERT(false);
			break;

		case kA3DTypeCrvOffset:
			A3DCrvOffsetGet(nullptr, (A3DCrvOffsetData *) pcCurveData);

			A3DEEntityType eTempEntityType = kA3DTypeUnknown;
			DWORD_PTR * pcTempEntityData = nullptr;
			ConvCurveBase::ClearCurveData(eRelationEntityType, pcRelationEntityData, eTempEntityType, pcTempEntityData);
			break;
	}

	if(nullptr != pcCurveData) {
		delete pcCurveData;
	}

	return true;
}

// == 2. Plane Curve 관련 함수 =======================================================================

// 2. Plane Curve 변환 함수
bool ConvCurveBase::ConvertPlaneCurve(const A3DCrvBase * pcCrvBase, const MbSurface * pcBaseSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve)
{
	A3DEEntityType eEntityType = kA3DTypeUnknown;
	CHECK_A3D_RESULT(A3DEntityGetType(pcCrvBase, &eEntityType));

	bool bStatus = false;
	switch(eEntityType)
	{
		/*
				case kA3DTypeCrvLine:
					bStatus = ConvertPlaneLine(pcCrvBase, pcSurface, dParentScale, pcPlaneCurve);
					break;

				case kA3DTypeCrvCircle:
					bStatus = ConvertPlaneCircle(pcCrvBase, pcSurface, dParentScale, pcPlaneCurve);
					break;
		*/

		case kA3DTypeCrvNurbs:
			bStatus = ConvertPlaneNurbs(pcCrvBase, pcBaseSurface, dParentScale, pcPlaneCurve);
			break;
	}

	if(false == bStatus) {
		CStringA strType = Dmi3dx::GetA3dEntityTypeString(eEntityType);
		LogManager::Log(2, "ConvertPlaneCurve Error: Type %s", strType);
	}

	return bStatus;
}

bool ConvCurveBase::ConvertPlaneCurveData(DWORD_PTR * pcCurveData, A3DEEntityType & eEntityType, const MbSurface * pcBaseSurface, double dParentScale, PlaneCurveSPtr & pcPlaneCurve)
{
	bool bStatus = false;
	switch(eEntityType)
	{
		/*
				case kA3DTypeCrvLine:
					bStatus = ConvertPlaneLine(pcCrvBase, pcSurface, dParentScale, pcPlaneCurve);
					break;

				case kA3DTypeCrvCircle:
					bStatus = ConvertPlaneCircle(pcCrvBase, pcSurface, dParentScale, pcPlaneCurve);
					break;
		*/

		case kA3DTypeCrvNurbs:
			bStatus = ConvertPlaneNurbsData(pcCurveData, pcBaseSurface, dParentScale, pcPlaneCurve);
			break;
	}

	if(false == bStatus) {
		CStringA strType = Dmi3dx::GetA3dEntityTypeString(eEntityType);
		LogManager::Log(2, "ConvertPlaneCurve Error: Type %s", strType);
	}

	return bStatus;
}

// 2-1. Plane Line 변환 함수 (사용된 적이 없음)
bool ConvCurveBase::ConvertPlaneLine(const A3DCrvBase * pcCrvBase, const MbSurface * pcSurface, double dParentScale, PlaneCurveSPtr & pcPlaneCurve)
{
	return false;

	A3DCrvLineData cCrvLineData;
	A3D_INITIALIZE_DATA(A3DCrvLineData, cCrvLineData);
	CHECK_A3D_RESULT(A3DCrvLineGet(pcCrvBase, &cCrvLineData));

	MbCartPoint cOirgin = Dmi3dx::GetMbCartPoint(cCrvLineData.m_sTrsf.m_sOrigin, dParentScale);
	MbVector cXAxis = Dmi3dx::GetMbVector(cCrvLineData.m_sTrsf.m_sXVector);

	double dMin = cCrvLineData.m_sParam.m_sInterval.m_dMin * dParentScale;;
	double dMax = cCrvLineData.m_sParam.m_sInterval.m_dMax * dParentScale;

	MbCartPoint cBegPoint = cOirgin + cXAxis * dMin;
	MbCartPoint cEndPoint = cOirgin + cXAxis * dMax;

	CalculateSurfaceCoeffPoint(pcSurface, cBegPoint);
	CalculateSurfaceCoeffPoint(pcSurface, cEndPoint);

	MbLineSegment * pcLine = new MbLineSegment(cBegPoint, cEndPoint);

	pcPlaneCurve = pcLine;

	return true;
}

// 2-2. Plane Circle 변환 함수 (사용된 적이 없음)
bool ConvCurveBase::ConvertPlaneCircle(const A3DCrvBase * pcCrvBase, const MbSurface * pcSurface, double dParentScale, PlaneCurveSPtr & pcPlaneCurve)
{
	ASSERT(false);

	A3DCrvCircleData cCrvCircleData;
	A3D_INITIALIZE_DATA(A3DCrvCircleData, cCrvCircleData);

	CHECK_A3D_RESULT(A3DCrvCircleGet(pcCrvBase, &cCrvCircleData));

	return true;
}

// 2-3. Plane Nurbs curve 변환 함수
bool ConvCurveBase::ConvertPlaneNurbs(const A3DCrvBase * pcCrvBase, const MbSurface * pcBaseSurface, double dParentScale, PlaneCurveSPtr & pcPlaneCurve)
{
	A3DCrvNurbsData cCrvNurbsData;
	A3D_INITIALIZE_DATA(A3DCrvNurbsData, cCrvNurbsData);
	CHECK_A3D_RESULT(A3DCrvNurbsGet(pcCrvBase, &cCrvNurbsData));

	return ConvertPlaneNurbsData((DWORD_PTR *) &cCrvNurbsData, pcBaseSurface, dParentScale, pcPlaneCurve);
}

// 2-3-1. Plane Nurbs curve 변환 Base 함수
bool ConvCurveBase::ConvertPlaneNurbsData(DWORD_PTR * pcCurveData, const MbSurface * pcBaseSurface, double dParentScale, PlaneCurveSPtr & pcPlaneCurve)
{
	A3DCrvNurbsData & cCrvNurbsData = *((A3DCrvNurbsData *) pcCurveData);

	ptrdiff_t nDegree = cCrvNurbsData.m_uiDegree;

	// Degree 1인 경우는 Line으로 처리한다. Degree 1인 경우는 MbNurbs를 생성하지 못한다.
	if(1 == nDegree && 2 == cCrvNurbsData.m_uiCtrlSize) {
		MbCartPoint cP1 = Dmi3dx::GetMbCartPoint(cCrvNurbsData.m_pCtrlPts[0], dParentScale);
		MbCartPoint cP2 = Dmi3dx::GetMbCartPoint(cCrvNurbsData.m_pCtrlPts[1], dParentScale);

		ConvCurveBase::CalculateSurfaceCoeffPoint(pcBaseSurface, cP1);
		ConvCurveBase::CalculateSurfaceCoeffPoint(pcBaseSurface, cP2);

		MbLineSegment * pcLine = new MbLineSegment(cP1, cP2);

		if(nullptr == pcLine) {
			return false;
		}

		pcPlaneCurve = pcLine;
		return true;
	}

	SArray<MbCartPoint> arcControlPointArray;
	SArray<double> ardWeightsArray;
	SArray<double> ardKnotsArray;

	// Control Point의 갯수는 Degree + 1이어야 함.
	for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiCtrlSize; nIndex++) {
		MbCartPoint cCartPoint = Dmi3dx::GetMbCartPoint(cCrvNurbsData.m_pCtrlPts[nIndex], dParentScale);

		ConvCurveBase::CalculateSurfaceCoeffPoint(pcBaseSurface, cCartPoint);

		arcControlPointArray.Add(cCartPoint);
	}

	if(0 == cCrvNurbsData.m_uiWeightSize) {
		for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiCtrlSize; nIndex++) {
			ardWeightsArray.Add(1.0);
		}
	}
	else {
		for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiWeightSize; nIndex++) {
			ardWeightsArray.Add(cCrvNurbsData.m_pdWeights[nIndex]);
		}
	}

	for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiKnotSize; nIndex++) {
		ardKnotsArray.Add(cCrvNurbsData.m_pdKnots[nIndex] * dParentScale); // 스케일을 곱해야 정상적인 값이 나옴.
	}

	// C3D에서는 Nrubs의 Degree가 Order로 되어 있음. 외부에 표시는 Degree로 표시되어 있음
	ptrdiff_t nSplineOrder = nDegree + 1;
	MbNurbs * pcNurbs = MbNurbs::Create(nSplineOrder, false, arcControlPointArray, ardWeightsArray, ardKnotsArray);
	if(nullptr == pcNurbs) {
		ASSERT(false);
		return false;
	}

	pcPlaneCurve = pcNurbs;

	return true;
}

bool ConvCurveBase::ConvertPlaneNurbsData(ConvEntityInfo & cEntityInfo, const MbSurface * pcBaseSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DCrvNurbsData & cCrvNurbsData = *((A3DCrvNurbsData *) cEntityInfo.pcEntityData[nTargetIndex]);

	return ConvertPlaneNurbsData((DWORD_PTR *) &cCrvNurbsData, pcBaseSurface, dParentScale, pcPlaneCurve);
}

// 2-4. 3D Curve를 Projection해서 Plance Curve 생성
bool ConvCurveBase::ConvertProjectionPlaneCurve(const A3DCrvBase * pcCrvBase, const MbSurface * pcSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve)
{
	SpaceCurveSPtr pcSpaceCurve;
	if(false == ConvertSpaceCurve(pcCrvBase, dParentScale, dParentScale, pcSpaceCurve)) {
		return false;
	}

	MbeSpaceType eSurfaceCurveType = pcSpaceCurve->IsA();
	MbeSpaceType eSurfaceType = pcSurface->IsA();

	SurfaceSPtr surface((MbSurface *) pcSurface);

	MbContour * pcContour = new MbContour();
	MbResultType eResultType = ::SurfaceBoundContour(*pcSurface, *pcSpaceCurve, Math::DefaultMathVersion(), pcContour);

	//if(st_Plane == eSurfaceType) 
	{
		MbPlane * pcPlane = (MbPlane *) pcSurface;
		MbPlacement3D curvePlace;
		pcPlane->GetPlanePlacement(curvePlace);

		MbCurve * curve = nullptr;
		if(true == pcSpaceCurve->GetPlaneCurve(curve, curvePlace, false)) {
			MbePlaneType ePlaneCurveType = curve->IsA();
			int i = 0;
		}
	}

	if(true == pcSpaceCurve->GetSurfaceCurve(pcPlaneCurve, surface)) {
		C3dTracer::PlaneItemInformation(*pcPlaneCurve, *surface);
		MbePlaneType ePlaneCurveType = pcPlaneCurve->IsA();
		return true;
	}

	return false;

	/*
		SurfaceSPtr surface(pcSurface);
		if(false == pcSpaceCurve->GetSurfaceCurve(pcPlaneCurve, surface)) {
			return false;
		}
	*/

	//MATH_FUNC(MbResultType) SurfaceBoundContour(const MbSurface & surface, const MbCurve3D & spaceCurve, VERSION version, MbContour * &result);

	//PlaneContourSPtr pcContour;
/*
	MbContour * pcContour = nullptr;
	MbResultType eResultType =::SurfaceBoundContour(*pcSurface, *pcSpaceCurve, Math::DefaultMathVersion(), pcContour);

	if(rt_Success != eResultType) {
		return false;
	}
*/

//MbePlaneType eType = pcPlaneCurve->IsA();

	int i = 0;

	//MbProjCurve

/*

	A3DEEntityType eEntityType = kA3DTypeUnknown;
	CHECK_A3D_RETURN(A3DEntityGetType(pcCrvBase, &eEntityType));

	bool bStatus = false;
	switch(eEntityType)
	{
		/ *
				case kA3DTypeCrvLine:
					bStatus = ConvertPlaneLine(pcCrvBase, pcSurface, dParentScale, pcPlaneCurve);
					break;

				case kA3DTypeCrvCircle:
					bStatus = ConvertPlaneCircle(pcCrvBase, pcSurface, dParentScale, pcPlaneCurve);
					break;
		* /

		case kA3DTypeCrvNurbs:
			bStatus = ConvertPlaneNurbs(pcCrvBase, pcBaseSurface, dParentScale, pcPlaneCurve);
			break;
	}

	if(false == bStatus) {
		CString strType = Dmi3dx::GetA3dEntityTypeString(eEntityType).c_str();
		LogManager::Log(2, L"ConvertPlaneCurve Error: Type %s", strType);
	}
*/

	return true;
}

bool ConvCurveBase::ConvertProjectionPlaneCurveData(DWORD_PTR * pcCurveData, A3DEEntityType & eEntityType, const MbSurface * pcBaseSurface, double dParentScale, c3d::PlaneCurveSPtr & pcPlaneCurve)
{
	return true;

}

// == 3. Space Curve 관련 함수 =======================================================================

// 3. Space Curve 생성 함수
bool ConvCurveBase::ConvertSpaceCurve(const A3DCrvBase * pcCrvBase, double dParentScale, double dContextScale, SpaceCurveSPtr & pcSpaceCurve)
{
	A3DEEntityType eEntityType = kA3DTypeUnknown;
	CHECK_A3D_RESULT(A3DEntityGetType(pcCrvBase, &eEntityType));

	bool bStatus = false;

	switch(eEntityType)
	{
		case kA3DTypeCrvLine:
			bStatus = ConvertSpaceLine(pcCrvBase, dParentScale, pcSpaceCurve);
			break;

		case kA3DTypeCrvPolyLine:
			bStatus = ConvertSpacePolyLine(pcCrvBase, dParentScale, pcSpaceCurve);
			break;

		case kA3DTypeCrvCircle:
			bStatus = ConvertSpaceCircle(pcCrvBase, dParentScale, pcSpaceCurve);
			break;

		case kA3DTypeCrvEllipse:
			bStatus = ConvertSpaceEllipse(pcCrvBase, dParentScale, pcSpaceCurve);
			break;

// 		case kA3DTypeCrvHyperbola:
// 			bStatus = ConvertSpaceGetAsNurbsCurve(pcCrvBase, dParentScale, dContextScale, pcSpaceCurve);
// 			break;

		case kA3DTypeCrvNurbs:
			bStatus = ConvertSpaceNurbs(pcCrvBase, dParentScale, dContextScale, pcSpaceCurve);
			break;

		case kA3DTypeCrvComposite:
			bStatus = ConvertSpaceCompositeCurve(pcCrvBase, dParentScale, dContextScale, pcSpaceCurve);
			break;

		case kA3DTypeCrvOnSurf:
		case kA3DTypeCrvOffset:
			bStatus = ConvertSpaceGetAsNurbsCurve(pcCrvBase, dParentScale, dContextScale, pcSpaceCurve);
			break;

		default:
			bStatus = ConvertSpaceGetAsNurbsCurve(pcCrvBase, dParentScale, dContextScale, pcSpaceCurve);
			//assert(false);
			break;
	}

	CStringA strType = Dmi3dx::GetA3dEntityTypeString(eEntityType);

	if(false == bStatus) {
		LogManager::Log(2, "ConvertSpaceCurve Error: Type %s", strType);
	}
	else {
		LogManager::Log(2, "ConvertSpaceCurve: Type %s", strType);
	}

	return bStatus;
}

bool ConvCurveBase::ConvertSpaceCurveData(ConvEntityInfo & cEntityInfo, double dParentScale, double dContextScale, SpaceCurveSPtr & pcSpaceCurve)
{
/*
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	bool bStatus = false;
	A3DEEntityType eCurveType = cEntityInfo.peEntityType[nTargetIndex];

	switch(eCurveType)
	{
		case kA3DTypeCrvLine:
			bStatus = ConvertSpaceLineData(cEntityInfo, dParentScale, pcSpaceCurve);
			break;

		case kA3DTypeCrvCircle:
			bStatus = ConvertSpaceCircleData(cEntityInfo, dParentScale, pcSpaceCurve);
			break;

		case kA3DTypeCrvEllipse:
			bStatus = ConvertSpaceEllipseData(cEntityInfo, dParentScale, pcSpaceCurve);
			break;

		case kA3DTypeCrvNurbs:
			bStatus = ConvertSpaceNurbsData(cEntityInfo, dParentScale, dContextScale, pcSpaceCurve);
			break;
	}

	if(false == bStatus) {
		CString strType = Dmi3dx::GetA3dEntityTypeString(eCurveType).c_str();
		LogManager::Log(2, L"GetCurveData Error: %s", strType);
		return false;
	}
*/

	return true;
}

bool ConvCurveBase::ConvertSpaceCurveData(A3DTopoWireEdgeData & cWireEdgeData, double dParentScale, double dContextScale, SpaceCurveSPtr & pcSpaceCurve)
{

	SpaceCurveSPtr pcConvertCurve;
	if(false == ConvCurveBase::ConvertSpaceCurve(cWireEdgeData.m_p3dCurve, dParentScale, dContextScale, pcConvertCurve)) {
		return false;
	}

	if(A3D_TRUE == cWireEdgeData.m_bHasTrimDomain) {
		pcSpaceCurve = pcConvertCurve->Trimmed(cWireEdgeData.m_sInterval.m_dMin, cWireEdgeData.m_sInterval.m_dMax, true);
	}
	else {
		pcSpaceCurve = pcConvertCurve;
	}

	return true;
}

// 3-1. Space Line 변환
bool ConvCurveBase::ConvertSpaceLine(const A3DCrvBase * pc3dCurve, double dParentScale, SpaceCurveSPtr & pcSpaceCurve)
{
	A3DCrvLineData cCrvLineData;
	A3D_INITIALIZE_DATA(A3DCrvLineData, cCrvLineData);
	CHECK_A3D_RESULT(A3DCrvLineGet(pc3dCurve, &cCrvLineData));

	MbPlacement3D cPlacement;
	Dmi3dx::GetPlacement3d(cCrvLineData.m_sTrsf, dParentScale, cPlacement);

	double dMin = cCrvLineData.m_sParam.m_sInterval.m_dMin * cCrvLineData.m_sTrsf.m_sScale.m_dX;
	double dMax = cCrvLineData.m_sParam.m_sInterval.m_dMax * cCrvLineData.m_sTrsf.m_sScale.m_dX;

	dMin *= cCrvLineData.m_sParam.m_dCoeffA;
	dMax *= cCrvLineData.m_sParam.m_dCoeffA;

	MbCartPoint3D cBegPoint = cPlacement.GetOrigin() + cPlacement.GetAxisX() * dMin;
	MbCartPoint3D cEndPoint = cPlacement.GetOrigin() + cPlacement.GetAxisX() * dMax;

	// LogManager::Log(2, L"ConvertSpaceLine: %f, %f, %f", cBegPoint.x, cBegPoint.y, cBegPoint.z);

	MbLineSegment3D * pcLine = new MbLineSegment3D(cBegPoint, cEndPoint);
	pcSpaceCurve = pcLine;

	A3DCrvLineGet(nullptr, &cCrvLineData);

	return true;
}

// 3-2. Space Polyline 변환
bool ConvCurveBase::ConvertSpacePolyLine(const A3DCrvBase * pc3dCurve, double dParentScale, SpaceCurveSPtr & pcSpaceCurve)
{
	A3DCrvPolyLineData cCrvPolyLineData;
	A3D_INITIALIZE_DATA(A3DCrvPolyLineData, cCrvPolyLineData);
	CHECK_A3D_RESULT(A3DCrvPolyLineGet(pc3dCurve, &cCrvPolyLineData));

	MbPlacement3D cPlacement;
	Dmi3dx::GetPlacement3d(cCrvPolyLineData.m_sTrsf, dParentScale, cPlacement);

	SArray<MbCartPoint3D> aPointArray;

	for(A3DUns32 nIndex = 0; nIndex < cCrvPolyLineData.m_uiSize; nIndex++) {
		MbCartPoint3D cPo = Dmi3dx::GetMbCartPoint3D(cCrvPolyLineData.m_pPts[nIndex], dParentScale);
		aPointArray.Add(cPo);
	}

	MbPolyline3D * pcPolyline3d = new MbPolyline3D(aPointArray, false);
	pcPolyline3d->Transform(MbMatrix3D(cPlacement));

	A3DCrvPolyLineGet(nullptr, &cCrvPolyLineData);

	pcSpaceCurve = pcPolyline3d;

	return true;
}

// 3-3. Space Circle 변환
bool ConvCurveBase::ConvertSpaceCircle(const A3DCrvBase * pc3dCurve, double dParentScale, SpaceCurveSPtr & pcSpaceCurve)
{
	A3DCrvCircleData cCrvCircleData;
	A3D_INITIALIZE_DATA(A3DCrvCircleData, cCrvCircleData);
	CHECK_A3D_RESULT(A3DCrvCircleGet(pc3dCurve, &cCrvCircleData));

	MbPlacement3D cPlacement;
	Dmi3dx::GetPlacement3d(cCrvCircleData.m_sTrsf, dParentScale, cPlacement);

	double dRadius = cCrvCircleData.m_dRadius * cCrvCircleData.m_sTrsf.m_sScale.m_dX * dParentScale;
	MbArc3D * pcArc3d = new MbArc3D(cPlacement, dRadius, dRadius, M_PI2);

	double dMin = cCrvCircleData.m_sParam.m_sInterval.m_dMin * cCrvCircleData.m_sParam.m_dCoeffA;// +cCrvCircleData.m_sParam.m_dCoeffB;
	double dMax = cCrvCircleData.m_sParam.m_sInterval.m_dMax * cCrvCircleData.m_sParam.m_dCoeffA;// +cCrvCircleData.m_sParam.m_dCoeffB;

	if(dMin < dMax) {
		pcSpaceCurve = pcArc3d->Trimmed(dMin, dMax, true);
	}
	else {
		pcArc3d->Inverse();
		pcSpaceCurve = pcArc3d->Trimmed(dMin, dMax, false);
	}

	// Trimed 되면서 복사됨.
	delete pcArc3d;

	A3DCrvCircleGet(nullptr, &cCrvCircleData);

	return true;
}

// 3-4. Space Ellipse 변환
bool ConvCurveBase::ConvertSpaceEllipse(const A3DCrvBase * pc3dCurve, double dParentScale, SpaceCurveSPtr & pcSpaceCurve)
{
	A3DCrvEllipseData cCrvEllipseData;
	A3D_INITIALIZE_DATA(A3DCrvEllipseData, cCrvEllipseData);
	CHECK_A3D_RESULT(A3DCrvEllipseGet(pc3dCurve, &cCrvEllipseData));

	ConvEntityInfo cEntityInfo(1);
	cEntityInfo.pcEntityData[0] = (DWORD_PTR *) &cCrvEllipseData;

	MbPlacement3D cPlacement;
	Dmi3dx::GetPlacement3d(cCrvEllipseData.m_sTrsf, dParentScale, cPlacement);

	double dRadiusX = cCrvEllipseData.m_dXRadius * cCrvEllipseData.m_sTrsf.m_sScale.m_dX;
	double dRadiusY = cCrvEllipseData.m_dYRadius * cCrvEllipseData.m_sTrsf.m_sScale.m_dY;
	MbArc3D * pcArc3d = new MbArc3D(cPlacement, dRadiusX, dRadiusY, M_PI2);

	double dMin = cCrvEllipseData.m_sParam.m_sInterval.m_dMin * cCrvEllipseData.m_sParam.m_dCoeffA;
	double dMax = cCrvEllipseData.m_sParam.m_sInterval.m_dMax * cCrvEllipseData.m_sParam.m_dCoeffA;

	if(dMin < dMax) {
		pcSpaceCurve = pcArc3d->Trimmed(dMin, dMax, true);
	}
	else {
		pcArc3d->Inverse();
		pcSpaceCurve = pcArc3d->Trimmed(dMin, dMax, false);
	}

	// Trimed 되면서 복사됨.
	delete pcArc3d;

	A3DCrvEllipseGet(nullptr, &cCrvEllipseData);

	return true;
}

// 3-5. Space Nurbs 변환
bool ConvCurveBase::ConvertSpaceNurbs(const A3DCrvBase * pc3dCurve, double dParentScale, double dContextScale, SpaceCurveSPtr & pcSpaceCurve)
{
	A3DCrvNurbsData cCrvNurbsData;
	A3D_INITIALIZE_DATA(A3DCrvNurbsData, cCrvNurbsData);
	CHECK_A3D_RESULT(A3DCrvNurbsGet(pc3dCurve, &cCrvNurbsData));

	bool bStatus = ConvCurveBase::ConvertSpaceNurbsData(cCrvNurbsData, dParentScale, dContextScale, pcSpaceCurve);

	A3DCrvNurbsGet(nullptr, &cCrvNurbsData);

	return bStatus;
}

bool ConvCurveBase::ConvertSpaceNurbsData(const A3DCrvNurbsData & cCrvNurbsData, double dParentScale, double dContextScale, SpaceCurveSPtr & pcSpaceCurve)
{
	ptrdiff_t nDegree = cCrvNurbsData.m_uiDegree;

	// Degree 1인 경우는 Line으로 처리한다. Degree 1인 경우는 MbNurbs를 생성하지 못한다.
	if(1 == nDegree && 2 == cCrvNurbsData.m_uiCtrlSize) {
		MbCartPoint3D cP1 = Dmi3dx::GetMbCartPoint3D(cCrvNurbsData.m_pCtrlPts[0], dParentScale);
		MbCartPoint3D cP2 = Dmi3dx::GetMbCartPoint3D(cCrvNurbsData.m_pCtrlPts[1], dParentScale);

		MbLineSegment3D * pcLine = new MbLineSegment3D(cP1, cP2);

		if(nullptr == pcLine) {
			return false;
		}

		pcSpaceCurve = pcLine;
		return true;
	}

	SArray<MbCartPoint3D> arcControlPointArray;
	SArray<double> ardWeightsArray;
	SArray<double> ardKnotsArray;

	// Control Point의 갯수는 Degree + 1이어야 함.
	for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiCtrlSize; nIndex++) {
		MbCartPoint3D cCartPoint = Dmi3dx::GetMbCartPoint3D(cCrvNurbsData.m_pCtrlPts[nIndex], dParentScale);
		arcControlPointArray.Add(cCartPoint);
	}

	if(0 == cCrvNurbsData.m_uiWeightSize) {
		for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiCtrlSize; nIndex++) {
			ardWeightsArray.Add(1.0);
		}
	}
	else {
		for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiWeightSize; nIndex++) {
			ardWeightsArray.Add(cCrvNurbsData.m_pdWeights[nIndex]);
		}
	}

	for(A3DUns32 nIndex = 0; nIndex < cCrvNurbsData.m_uiKnotSize; nIndex++) {
		double dKnots = cCrvNurbsData.m_pdKnots[nIndex] * dParentScale;

		// Context sclae을 적용해야 정확한 값이 나옴. 스케일을 곱해야 정상적인 값이 나옴.
		ardKnotsArray.Add(cCrvNurbsData.m_pdKnots[nIndex] * dContextScale);
	}

	// C3D에서는 Nrubs의 Degree가 Order로 되어 있음. 외부에 표시는 Degree로 표시되어 있음
	ptrdiff_t nSplineOrder = nDegree + 1;

	MbNurbs3D * pcNurbs = MbNurbs3D::Create(nSplineOrder, false, arcControlPointArray, ardWeightsArray, ardKnotsArray);
	if(nullptr == pcNurbs) {
		ASSERT(false);
		return false;
	}

	pcSpaceCurve = pcNurbs;

	return true;
}

// 3-6. Space Composite Curve 변환
bool ConvCurveBase::ConvertSpaceCompositeCurve(const A3DCrvBase * pc3dCurve, double dParentScale, double dContxtScale, SpaceCurveSPtr & pcSpaceCurve)
{
	LogManager::Log(2, L"CrvComposite");

	LogManager::IncreaseTabIndex(2);

	A3DCrvCompositeData cCrvCompositeData;
	A3D_INITIALIZE_DATA(A3DCrvCompositeData, cCrvCompositeData);
	CHECK_A3D_RESULT(A3DCrvCompositeGet(pc3dCurve, &cCrvCompositeData));

	SpaceCurvesSPtrVector cCurveVector;

	for(A3DUns32 nIndex = 0; nIndex < cCrvCompositeData.m_uiSize; nIndex++) {
		// #import_debug : Composite curve Index
//  	if(5 != nIndex) {
//  		continue;
//  	}

		SpaceCurveSPtr pcConvertCurve;
		if(true == ConvertSpaceCurve(cCrvCompositeData.m_ppCurves[nIndex], dParentScale, dContxtScale, pcConvertCurve)) {
			cCurveVector.push_back(pcConvertCurve);
		}
	}

	if(true == cCurveVector.empty()) {
		LogManager::DecreaseTabIndex(2);
		return false;
	}

	MbPlacement3D cPlacement;
	Dmi3dx::GetPlacement3d(cCrvCompositeData.m_sTrsf, dParentScale, cPlacement);

	MbMatrix3D cMatrix(cPlacement);

	MbContour3D * pcContour = new MbContour3D();

	for(size_t nIndex = 0; nIndex < cCurveVector.size(); nIndex++) {
		SpaceCurveSPtr pcCurve = cCurveVector[nIndex];
		bool bCurveSenseFlag = true;
		if(A3D_FALSE == cCrvCompositeData.m_pbSenses[nIndex]) {
			bCurveSenseFlag = false;
		}

		pcContour->AddSegment(*pcCurve, bCurveSenseFlag);
	}

	pcContour->Transform(cMatrix);

	pcSpaceCurve = pcContour;

	A3DCrvCompositeGet(nullptr, &cCrvCompositeData);

	LogManager::DecreaseTabIndex(2);

	return true;
}

// 3-7. 주어진 Curve를 Nurbs Curve로 바꾸어서 전달.
bool ConvCurveBase::ConvertSpaceGetAsNurbsCurve(const A3DCrvBase * pc3dCurve, double dParentScale, double dContextScale, SpaceCurveSPtr & pcSpaceCurve)
{
	double dTolerance = ConvCurveBase::dTolerance / dContextScale;

	A3DCrvNurbsData cCrvNurbsData;
	A3D_INITIALIZE_DATA(A3DCrvNurbsData, cCrvNurbsData);
	CHECK_A3D_RESULT(A3DCrvBaseGetAsNurbs(pc3dCurve, dTolerance, true, &cCrvNurbsData));

	return ConvertSpaceNurbsData(cCrvNurbsData, dParentScale, dContextScale, pcSpaceCurve);
}

// == Utility =======================================================================================

// 주기성 곡면에서 Coefficent가 적용된 Point값을 계산하는 함수
void ConvCurveBase::CalculateSurfaceCoeffPoint(const MbSurface * pcSurface, MbCartPoint & cPoint)
{
	switch(pcSurface->IsA())
	{
		case st_CylinderSurface:
		case st_ConeSurface:
		case st_SphereSurface:
		case st_TorusSurface:
		case st_RuledSurface:
		case st_ElevationSurface:
		case st_RevolutionSurface:
		case st_ExtrusionSurface:
		{
			const DmiSurface * pcDmiSurface = dynamic_cast<const DmiSurface *>(pcSurface);
			pcDmiSurface->CheckPoint(cPoint);
			bool bSwapUVFlag = pcDmiSurface->IsSwapUV();
			if(true == bSwapUVFlag) {
				std::swap(cPoint.x, cPoint.y);
			}
		}
		break;

		case st_Plane:
		{
			const DmiSurface * pcDmiSurface = dynamic_cast<const DmiSurface *>(pcSurface);
			pcDmiSurface->CheckPoint(cPoint);
		}
		break;

		case st_CurveBoundedSurface:
		{
			const MbCurveBoundedSurface * pcCurveBoundedSurface = dynamic_cast<const MbCurveBoundedSurface *>(pcSurface);
			CalculateSurfaceCoeffPoint(&pcCurveBoundedSurface->GetBasisSurface(), cPoint);
		}
		break;
	}

	// V값이 범위안에 있는지 여부 확인 (Major radius가 Minor radius보다 작은 경우 확인 필요, 완전한 Torus가 아닌 형태)

	// #Check_point : Torus Radius 관련 처리 부분 일단 Remark한다.
/*
	if(st_TorusSurface == pcSurface->IsA()) {
		MbTorusSurface * pcTorus = (MbTorusSurface *) pcSurface;
		if(pcTorus->GetMajorRadius() < pcTorus->GetMinorRadius()) {
			double dVMax = pcTorus->GetVMax();
			double dVMin = pcTorus->GetVMin();

			if(cPoint.y < dVMin || cPoint.y > dVMax) {
				cPoint.y -= ::floor((cPoint.y - dVMin) * Math::invPI2) * M_PI2;
			}
		}
	}*/
}

// == 과거 개발 함수 ==================================================================================

namespace ConvCurveBaseOld
{
	bool ConvertSpaceOnSurfCurve(const A3DCrvBase * pc3dCurve, double dParentScale, double dContextScale, SpaceCurveSPtr & pcSpaceCurve);
};

bool ConvCurveBaseOld::ConvertSpaceOnSurfCurve(const A3DCrvBase * pc3dCurve, double dParentScale, double dContextScale, SpaceCurveSPtr & pcSpaceCurve)
{
	/*
		A3DCrvOnSurfData cCrvOnSurfData;
		A3D_INITIALIZE_DATA(A3DCrvOnSurfData, cCrvOnSurfData);
		CHECK_A3D_RETURN(A3DCrvOnSurfGet(pc3dCurve, &cCrvOnSurfData));

		A3DEEntityType eSurfaceType;
		A3DEntityGetType(cCrvOnSurfData.m_pSurface, &eSurfaceType);
		LogManager::Log(2, L"CrvOnSurf: %s", Rt3dx::GetA3dEntityTypeString(eSurfaceType).c_str());

		LogManager::IncreaseTabIndex(2);

		double dScale = cCrvOnSurfData.m_sTrsf.m_sScale.m_dX * dParentScale;

		// Surface 정보 수집
		SurfaceSPtr pcSurface;
		if(false == ConvSurfaceBase::ConvertSurfBase(cCrvOnSurfData.m_pSurface, dScale, dContextScale, pcSurface)) {
			A3DCrvOnSurfGet(nullptr, &cCrvOnSurfData);
			return false;
		}

		if(nullptr == pcSurface) {
			A3DCrvOnSurfGet(nullptr, &cCrvOnSurfData);
			return false;
		}

		// UV Curve 정보 수집
		PlaneCurveSPtr pcPlaneCurve;
		if(false == ConvCurveBase::ConvertPlaneCurve(cCrvOnSurfData.m_pUVCurve, pcSurface, dScale, pcPlaneCurve)) {
			A3DCrvOnSurfGet(nullptr, &cCrvOnSurfData);
			return false;
		}

		if(nullptr == pcPlaneCurve) {
			A3DCrvOnSurfGet(nullptr, &cCrvOnSurfData);
			return false;
		}

		SurfaceSPtr pcBoundSurface;
		if(true == ConvSurfaceBase::CreateBoundedSurface(pcSurface, pcPlaneCurve, pcBoundSurface)) {
			pcSurface = pcBoundSurface;
		}

		MbSurfaceCurve * pcSurfaceCurve = new MbSurfaceCurve(*pcSurface, *pcPlaneCurve, true);
		if(nullptr == pcSurfaceCurve) {
			A3DCrvOnSurfGet(nullptr, &cCrvOnSurfData);
			return false;
		}

		double dMin = cCrvOnSurfData.m_sParam.m_sInterval.m_dMin * dScale;
		double dMax = cCrvOnSurfData.m_sParam.m_sInterval.m_dMax * dScale;
		pcSpaceCurve = pcSurfaceCurve->Trimmed(dMin, dMax, true);

		LogManager::DecreaseTabIndex(2);
	*/

	return true;
}

// == 3. Utitliy Function ==========================================================================
bool ConvCurveBase::GetTargetEntityIndex(ConvEntityInfo & cEntityInfo, int & nTargetIndex)
{
	nTargetIndex = cEntityInfo.nTargetEntityIndex;
	if(nullptr == cEntityInfo.pcEntityData[nTargetIndex]) {
		ASSERT(false);
		return false;
	}

	return true;
}

bool ConvCurveBase::GetCurveCoeffcient(ConvEntityInfo & cEntityInfo, double & dCoeffA, double & dCoeffB)
{
	int nTargetIndex = -1;
	if(false == GetTargetEntityIndex(cEntityInfo, nTargetIndex)) {
		return false;
	}

	A3DCrvNurbsData & cCrvNurbsData = *((A3DCrvNurbsData *) cEntityInfo.pcEntityData[nTargetIndex]);

	switch(cEntityInfo.peEntityType[nTargetIndex])
	{
		case kA3DTypeCrvLine:
		{
			A3DCrvLineData & cCrvLineData = *((A3DCrvLineData *) cEntityInfo.pcEntityData[nTargetIndex]);
			dCoeffA = cCrvLineData.m_sParam.m_dCoeffA;
			dCoeffB = cCrvLineData.m_sParam.m_dCoeffB;
		}
		break;

		case kA3DTypeCrvCircle:
		{
			A3DCrvCircleData & cCrvCircleData = *((A3DCrvCircleData *) cEntityInfo.pcEntityData[nTargetIndex]);
			dCoeffA = cCrvCircleData.m_sParam.m_dCoeffA;
			dCoeffB = cCrvCircleData.m_sParam.m_dCoeffB;

		}
		break;

		case kA3DTypeCrvEllipse:
		{
			A3DCrvEllipseData & cCrvEllipseData = *((A3DCrvEllipseData *) cEntityInfo.pcEntityData[nTargetIndex]);
			dCoeffA = cCrvEllipseData.m_sParam.m_dCoeffA;
			dCoeffB = cCrvEllipseData.m_sParam.m_dCoeffB;
		}
		break;
	}

	return true;
}