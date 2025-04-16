#include "stdafx.h"
#include "ConvCoEdge.h"

#include "ConvEdge.h"
#include "ConvCurve.h"
#include "ConvVertex.h"

#include <cur_surface_intersection.h>

#define CHECK_BOOL_RETURN(FunctionCall) { bool nResult = FunctionCall; if (nResult == false) { return false; } }
#define CHECK_A3D_RESULT(FunctionCall) { A3DStatus eResult = FunctionCall; if (eResult != A3D_SUCCESS) { return false; } }

ConvCoEdge::ConvCoEdge(A3DTopoCoEdge * pcTopoCoEdge, A3DSurfBase * pcSurfBase, double dContextScale)
{
	if(nullptr == pcTopoCoEdge) {
		return;
	}

	m_pcTopoCoEdge = pcTopoCoEdge;

	m_pcSurfBase = pcSurfBase;

	// Loop Data 저장
	A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, m_cTopoCoEdgeData);
	A3DStatus eResult = A3DTopoCoEdgeGet(pcTopoCoEdge, &m_cTopoCoEdgeData);
	if(A3D_SUCCESS != eResult) {
		LogManager::Log(2, L"Error - ConvCoEdge/A3DTopoCoEdgeGet");
		return;
	}

	if(false == GetEdgeData(m_cTopoCoEdgeData.m_pEdge)) {
		LogManager::Log(2, L"Error - ConvCoEdge/GetEdgeData");
		A3DTopoCoEdgeGet(nullptr, &m_cTopoCoEdgeData);
		return;
	}

	if(false == GetCurveData(m_cTopoCoEdgeData.m_pUVCurve, dContextScale, m_pcCurve)) {
		LogManager::Log(2, L"Error - ConvCoEdge/GetCurveData");
		A3DTopoCoEdgeGet(nullptr, &m_cTopoCoEdgeData);
		return;
	}

	// ----- 이웃 CoEdge 정보 수집 -----
	m_pcNeighborTopoCoEdge = m_cTopoCoEdgeData.m_pNeighbor;

	m_bInitFlag = true;
}

ConvCoEdge::ConvCoEdge(A3DTopoCoEdge * pcTopoCoEdge, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap & mpcConvSurfaceMap)
{
	/*if(nullptr == pcTopoCoEdge) {
		return;
	}

	m_pcTopoCoEdge = pcTopoCoEdge;

	// Loop Data 저장
	A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, m_cTopoCoEdgeData);
	A3DStatus eResult = A3DTopoCoEdgeGet(pcTopoCoEdge, &m_cTopoCoEdgeData);
	if(A3D_SUCCESS != eResult) {
		LogManager::Log(2, L"Error - ConvCoEdge/A3DTopoCoEdgeGet");
		return;
	}

	if(false == GetEdgeData(m_cTopoCoEdgeData.m_pEdge)) {
		LogManager::Log(2, L"Error - ConvCoEdge/GetEdgeData");
		A3DTopoCoEdgeGet(nullptr, &m_cTopoCoEdgeData);
		return;
	}

	if(false == GetCurveData(m_cTopoCoEdgeData.m_pUVCurve, dContextScale, m_pcCurve)) {
		LogManager::Log(2, L"Error - ConvCoEdge/GetCurveData");
		A3DTopoCoEdgeGet(nullptr, &m_cTopoCoEdgeData);
		return;
	}

	// ----- 이웃 CoEdge 정보 수집 -----
	A3DTopoCoEdge * pcNeighborTopoCoEdge = m_cTopoCoEdgeData.m_pNeighbor;
	if(nullptr != pcNeighborTopoCoEdge) {
		// 이웃 CoEdge Surface 정보 수집
		GetNeighborSurface(pcNeighborTopoCoEdge, dContextScale, pcBrepData, mpcConvSurfaceMap);

		// 이웃 CoEdge의 정보를 가져옴
		A3DTopoCoEdgeData cNeighborTopoCoEdgeData;
		A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, cNeighborTopoCoEdgeData);
		A3DStatus eResult = A3DTopoCoEdgeGet(pcNeighborTopoCoEdge, &cNeighborTopoCoEdgeData);
		if(A3D_SUCCESS != eResult) {
			A3DTopoCoEdgeGet(nullptr, &m_cTopoCoEdgeData);
			LogManager::Log(2, L"Error - ConvCoEdge/Neighbor A3DTopoCoEdgeGet");
			return;
		}

		// 이웃 UV Curve Base 저장
		A3DCrvBase * pcCrvBase = cNeighborTopoCoEdgeData.m_pUVCurve;
		A3DTopoCoEdgeGet(nullptr, &cNeighborTopoCoEdgeData);

		// 이웃 CoEdge Curve 정보 수집
		if(false == GetCurveData(pcCrvBase, dContextScale, m_pcNeighborCurve)) {
			LogManager::Log(2, L"Error - ConvCoEdge/Neighbor GetCurveData");
			A3DTopoCoEdgeGet(nullptr, &m_cTopoCoEdgeData);
			return;
		}
	}
*/

	m_bInitFlag = true;
}

ConvCoEdge::~ConvCoEdge()
{
	if(true == m_bInitFlag) {
		A3DTopoCoEdgeGet(nullptr, &m_cTopoCoEdgeData);
	}

	if(nullptr != m_pcConvEdge) {
		delete m_pcConvEdge;
	}
}

bool ConvCoEdge::Convert()
{
	bool bOrientationWithLoopFlag = (1 == m_cTopoCoEdgeData.m_ucOrientationWithLoop) ? true : false;
	bool bOrientationUVWithLoop = (1 == m_cTopoCoEdgeData.m_ucOrientationUVWithLoop) ? true : false;

	c3d::SurfaceSPtr pcNeighborSurface;
	c3d::PlaneCurveSPtr pcNeighborPlaneCurve;

	if(nullptr != m_pcNeighborConvCoEdge) {
		pcNeighborSurface = m_pcNeighborConvCoEdge->GetSurface();
		pcNeighborPlaneCurve = m_pcNeighborConvCoEdge->GetPlaneCurve();
	}

	bool bFlag1 = true, bFlag2 = true;

	c3d::IntersectionCurveSPtr pcInterCurve;
	if(nullptr != pcNeighborSurface && nullptr != pcNeighborPlaneCurve) { // 이웃 Face가 있는 경우 처리
		pcInterCurve = new MbSurfaceIntersectionCurve(*m_pcSurface, *m_pcPlaneCurve,
			*pcNeighborSurface, *pcNeighborPlaneCurve, MbeCurveBuildType::cbt_Ordinary, bFlag1, bFlag2);
	}
	else {
		//  Convert Ri에서 시작된 Face들은 Face Set이거나 Solid 구성이 아니기 때문에, 이웃 CorEdge가 없다.
		pcInterCurve = new MbSurfaceIntersectionCurve(*m_pcSurface, *m_pcPlaneCurve,
			*m_pcSurface, *m_pcPlaneCurve, MbeCurveBuildType::cbt_Ordinary, bFlag1, bFlag2);
	}

	if(bOrientationUVWithLoop != bOrientationWithLoopFlag) {
		pcInterCurve->Inverse();
	}

	// 4. Vertex 정보를 검색하고, 저장된 값이 없으면 생성한다.
	ConvVertex * pcStartConvVertex = m_pcConvEdge->GetStartConvVertex();
	ConvVertex * pcEndConvVertex = m_pcConvEdge->GetEndConvVertex();

	if(nullptr != pcStartConvVertex && nullptr != pcEndConvVertex) {
		c3d::VertexSPtr pcStartVertex = pcStartConvVertex->GetVertex();
		c3d::VertexSPtr pcEndVertex = pcEndConvVertex->GetVertex();

		m_pcCurveEdge = new MbCurveEdge(*pcStartVertex, *pcEndVertex, *pcInterCurve, true);
	}
	else {
		m_pcCurveEdge = new MbCurveEdge(*pcInterCurve, true);
	}

	if(nullptr == m_pcCurveEdge) {
		ASSERT(false);
		return false;
	}

	m_pcOrientedEdge = new MbOrientedEdge(*m_pcCurveEdge, bOrientationWithLoopFlag);
	if(nullptr == m_pcOrientedEdge) {
		ASSERT(false);
		return false;
	}

	return true;
}

// 내부 변수들을 이용해서, C3D Entity를 생성
bool ConvCoEdge::ConvertCurve(double dContextScale)
{
	// Curve convert
	return m_pcCurve->ConvertPlaneCurve(m_pcSurface, dContextScale, m_pcPlaneCurve);
}

bool ConvCoEdge::GetEdgeData(A3DTopoEdge * pcTopoEdge)
{
	m_pcConvEdge = new ConvEdge(pcTopoEdge);
	if(nullptr == m_pcConvEdge) {
		return false;
	}

	if(false == m_pcConvEdge->IsInit()) {
		delete m_pcConvEdge;
		return false;
	}

	return true;
}

// UV Curve 정보 수집
bool ConvCoEdge::GetCurveData(A3DCrvBase * pcCrvBase, double dContextScale, ConvCurve *& pcCurve)
{
	pcCurve = new ConvCurve(pcCrvBase, dContextScale);
	if(nullptr == pcCurve) {
		return false;
	}

	if(false == pcCurve->IsInit()) {
		return false;
	}

	return true;
}

// TopoVertex 정보 수집
void ConvCoEdge::CollectTopoVertex(ConvVertexMap & mpcConvVertexAtlMap)
{
	if(nullptr == m_pcConvEdge) {
		return;
	}

	m_pcConvEdge->CollectTopoVertex(mpcConvVertexAtlMap);
}

// == Neighbor CoEdge 관련 정보 ======================================================================
bool ConvCoEdge::GetNeighborSurface(A3DTopoCoEdge * pcTopoCoEdge, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap mpcConvSurfaceMap)
{
	const A3DSurfBase * pcSurfBase = nullptr;
	if(false == GetSurfBaseFromTopoCoEdge(pcTopoCoEdge, pcBrepData, pcSurfBase)) {
		return false;
	}

	if(false == GetConvSurfaceData(pcSurfBase, dContextScale, mpcConvSurfaceMap, m_pcNeighborSurface)) {
		return false;
	}

	return true;
}

// == Uility 함수 ===================================================================================

// U-1. 주어진 CoEdge의 Surface를 찾아오는 함수 (A3D 함수 이용)
bool ConvCoEdge::GetSurfBaseFromTopoCoEdge(const A3DTopoCoEdge * psCoEdge, const A3DTopoBrepData * pcBrepData, const A3DSurfBase *& pcSurfBase)
{
	const A3DTopoFace * pcTopoFace = nullptr;
	if(false == GetTopoFaceFromTopoCoEdge(psCoEdge, pcBrepData, pcTopoFace)) {
		return false;
	}

	if(false == GetSurfBaseFromTopoFace(pcTopoFace, pcSurfBase)) {
		return false;
	}

	return true;
}

// U-2. 주어진 CoEdge의 Face를 찾아오는 함수 (A3D 함수 이용)
bool ConvCoEdge::GetTopoFaceFromTopoCoEdge(const A3DTopoCoEdge * psCoEdge, const A3DTopoBrepData * pcBrepData, const A3DTopoFace *& pcTopoFace)
{
	if(nullptr == pcBrepData) {
		return false;
	}

	A3DTopoLoop * pcTopoLoop = nullptr;
	A3DStatus eResult = A3DTopoCoedgeGetLoop(psCoEdge, &pcTopoLoop);
	if(A3D_SUCCESS != eResult) {
		return false;
	}

	eResult = A3DTopoLoopGetFace(pcBrepData, pcTopoLoop, &pcTopoFace);
	if(A3D_SUCCESS != eResult) {
		return false;
	}

	if(nullptr == pcTopoFace) {
		return false;
	}

	return true;
}

// U-3. 주어진 Face에서 Surface를 찾아오는 함수
bool ConvCoEdge::GetSurfBaseFromTopoFace(const A3DTopoFace * pcTopoFace, const A3DSurfBase *& pcSurfBase)
{
	A3DTopoFaceData cTopoFaceData;
	A3D_INITIALIZE_DATA(A3DTopoFaceData, cTopoFaceData);
	A3DStatus eResult = A3DTopoFaceGet(pcTopoFace, &cTopoFaceData);

	if(A3D_SUCCESS != eResult) {
		//SetLastErrorMessage(L"ConvertTopoCoEdge - TopoFaceGet Error", eResult);
		return false;
	}

	pcSurfBase = cTopoFaceData.m_pSurface;

	// 메모리 해제
	A3DTopoFaceGet(nullptr, &cTopoFaceData);

	return true;
}