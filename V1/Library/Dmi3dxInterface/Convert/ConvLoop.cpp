#include "stdafx.h"
#include "ConvLoop.h"

#include "ConvSurface.h"
#include "ConvCoEdge.h"

#include <cur_line_segment.h>
#include <cur_surface_intersection.h>

#define CHECK_BOOL_RETURN(FunctionCall) { bool nResult = FunctionCall; if (nResult == false) { return false; } }
#define CHECK_A3D_RETURN(FunctionCall) { A3DStatus eResult = FunctionCall; if (eResult != A3D_SUCCESS) { return false; } }

ConvLoop::ConvLoop(A3DTopoLoop * pcTopoLoop, A3DSurfBase * pcSurfBase, double dContextScale)
{
	// Loop Data 저장
	A3D_INITIALIZE_DATA(A3DTopoLoopData, m_cTopoLoopData);
	A3DStatus eResult = A3DTopoLoopGet(pcTopoLoop, &m_cTopoLoopData);
	if(A3D_SUCCESS != eResult) {
		LogManager::Log(2, L"Error - ConvLoop/A3DTopoLoopGet");
		return;
	}

	for(A3DUns32 nIndex = 0; nIndex < m_cTopoLoopData.m_uiCoEdgeSize; nIndex++) {
		if(false == GetCoEdgeData(m_cTopoLoopData.m_ppCoEdges[nIndex], pcSurfBase, dContextScale)) {
			LogManager::Log(2, L"Error - ConvLoop/GetCoEdgeData");
			return;
		}
	}

	m_bInitFlag = true;
}

ConvLoop::ConvLoop(A3DTopoLoop * pcTopoLoop, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap & mpcConvSurfaceMap)
{
	// Loop Data 저장
	A3D_INITIALIZE_DATA(A3DTopoLoopData, m_cTopoLoopData);
	A3DStatus eResult = A3DTopoLoopGet(pcTopoLoop, &m_cTopoLoopData);
	if(A3D_SUCCESS != eResult) {
		LogManager::Log(2, L"Error - ConvLoop/A3DTopoLoopGet");
		return;
	}

	for(A3DUns32 nIndex = 0; nIndex < m_cTopoLoopData.m_uiCoEdgeSize; nIndex++) {
		if(false == GetCoEdgeData(m_cTopoLoopData.m_ppCoEdges[nIndex], dContextScale, pcBrepData, mpcConvSurfaceMap)) {
			LogManager::Log(2, L"Error - ConvLoop/GetCoEdgeData");
			return;
		}
	}

	m_bInitFlag = true;
}

ConvLoop::~ConvLoop()
{
	if(true == m_bInitFlag) {
		A3DTopoLoopGet(nullptr, &m_cTopoLoopData);
	}
}

// 1. 변환
bool ConvLoop::Convert(ConvSurface * pcConvSurface, bool bOrientationWithShell, bool bFirstLoopFlag)
{
	// Loop Sense 설정
	bool bOrientationWithSurfaceFlag = (1 == m_cTopoLoopData.m_ucOrientationWithSurface) ? true : false;

	for(auto pcConvCoEdge : m_vpcConvCoEdgeVector) {
		pcConvCoEdge->Convert();
	}

	std::list<const MbOrientedEdge *> lpcOrientedEdgeList;
	for(ConvCoEdge * pcConvCoEdge : m_vpcConvCoEdgeVector) {
		if(nullptr != pcConvCoEdge->GetOrientedEdge()) {
			lpcOrientedEdgeList.push_back(pcConvCoEdge->GetOrientedEdge());
		}
	};

	// 생성된 OrientEdge 크기를 확인
	if(0 == lpcOrientedEdgeList.size()) {
		ASSERT(false);
		return false;
	}

	// Loop가 Open되어 있는지 여부를 확인해서 Open되어 있는 경우 Close되도록 Edge를 추가한다.
	// Cone Surface에 대해서만 확인한다. (Cone은 Pole까지 Surface가 있는 경우 Curve가 Open됨)
	MbSurface * pcSurface = pcConvSurface->GetSurface();
	if(nullptr == pcSurface) {
		ASSERT(false);
		return false;
	}

	MbeSpaceType eSpaceType = pcSurface->IsA();

	if(st_ConeSurface == eSpaceType) {
		CheckOrientedEdgeOpenAndHealing(lpcOrientedEdgeList, pcSurface, 1.0e-6);
	}
	// #Chech_point
/*
	else {
		// ConeSurface가 아닌 경우에서 총 2개의 Edge Curve가 모드 line인 경우 Loop를 만들지 않는다.
		// 이것은 첫번째 Loop에 대해서만 적용하도록 한다.
		if(true == bFirstLoopFlag && 2 == lpcOrientedEdgeList.size()) {
			if(true == IsOrientedEdgeAllPlaneLine(lpcOrientedEdgeList, pcSurface)) {
				ASSERT(false);
				return false;
			}
		}
	}

*/
	m_pcLoop = new MbLoop();
	if(nullptr == m_pcLoop) {
		ASSERT(false);
		return false;
	}

	for(const MbOrientedEdge * pcOrientedEdge : lpcOrientedEdgeList) {
		m_pcLoop->AddEdge(*pcOrientedEdge);
	}

	if(bOrientationWithShell != bOrientationWithSurfaceFlag) {
		m_pcLoop->PartialReverse();
	}

	return true;
}

// 2. ConvCoEdge 정보 수집
void ConvLoop::CollectConvCoEdge(ConvCoEdgeMap & mpcConvCoEdgeAtlMap)
{
	for(auto pcCoEdge : m_vpcConvCoEdgeVector) {
		// 찾지 못한 경우에만 추가
		ConvCoEdge * pcConvCoEdge = nullptr;
		if(false == mpcConvCoEdgeAtlMap.Lookup((DWORD_PTR) pcCoEdge->GetTopoCoEdge(), pcConvCoEdge)) {
			mpcConvCoEdgeAtlMap.SetAt((DWORD_PTR) pcCoEdge->GetTopoCoEdge(), pcCoEdge);
		}
	}
}

// 3. TopoVertex 정보 수집
void ConvLoop::CollectTopoVertex(ConvVertexMap & mpcConvVertexAtlMap)
{
	for(auto pcCoEdge : m_vpcConvCoEdgeVector) {
		pcCoEdge->CollectTopoVertex(mpcConvVertexAtlMap);
	}
}


bool ConvLoop::GetCoEdgeData(A3DTopoCoEdge * pcTopoCoEdge, A3DSurfBase * pcSurfBase, double dContextScale)
{
	ConvCoEdge * pcCoEdge = new ConvCoEdge(pcTopoCoEdge, pcSurfBase, dContextScale);
	if(nullptr == pcCoEdge) {
		return false;
	}

	if(false == pcCoEdge->IsInit()) {
		return false;
	}

	m_vpcConvCoEdgeVector.push_back(pcCoEdge);

	return true;
}

bool ConvLoop::GetCoEdgeData(A3DTopoCoEdge * pcTopoCoEdge, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap & mpcConvSurfaceMap)
{
	ConvCoEdge * pcCoEdge = new ConvCoEdge(pcTopoCoEdge, dContextScale, pcBrepData, mpcConvSurfaceMap);
	if(nullptr == pcCoEdge) {
		return false;
	}

	if(false == pcCoEdge->IsInit()) {
		return false;
	}

	m_vpcConvCoEdgeVector.push_back(pcCoEdge);

	return true;
}

// == 4. Utility 함수 ===============================================================================

// 4. OrientedEdge의 Open 여부를 확인하고 Open된 경우 Close되도록 Edge를 추가한다.
void ConvLoop::CheckOrientedEdgeOpenAndHealing(OrientedEdgeList & lpcOrientedEdgeList, const MbSurface * pcBaseSurface, double dTolerance)
{
	auto cIterator = lpcOrientedEdgeList.begin();
	auto cCurIterator = cIterator;
	auto cNextIterator = cIterator;
	auto cEndIterator = std::prev(lpcOrientedEdgeList.end());

	const MbRect cRect;

	// 중간에 삽입되어도 순회하는데 문제는 없음. [2022/01/24 11:11 sysong]
	while(cIterator != lpcOrientedEdgeList.end()) {
		const MbOrientedEdge * pcCurOrientedEdge = *cIterator;
		const MbOrientedEdge * pcNextOrientedEdge = nullptr;

		cCurIterator = cIterator;
		++cIterator;
		cNextIterator = cIterator;

		// 마지막 요소라면 처음 요소와 비교한다.
		if(cCurIterator == cEndIterator) {
			pcNextOrientedEdge = *lpcOrientedEdgeList.begin();
		}
		else {
			pcNextOrientedEdge = *cIterator;
		}

		bool bConnectFlag = false;
		MbCartPoint cStartPoint, cEndPoint;

		if(true == CheckConectivity(pcCurOrientedEdge, pcNextOrientedEdge, pcBaseSurface, dTolerance, bConnectFlag)) {
			if(false == bConnectFlag) {
				const MbOrientedEdge * pcCreateOrientedEdge = nullptr;
				if(true == CreateLineSegmentOrientedEdge(lpcOrientedEdgeList, pcCurOrientedEdge, pcNextOrientedEdge, pcBaseSurface, dTolerance,
					pcCreateOrientedEdge))
				{
					if(cCurIterator == cEndIterator) {
						lpcOrientedEdgeList.push_back(pcCreateOrientedEdge);
					}
					else {
						lpcOrientedEdgeList.insert(cIterator, pcCreateOrientedEdge);
					}
				}
			}
		}
	}
}

bool ConvLoop::CheckConectivity(const MbOrientedEdge * pcCurEdge, const MbOrientedEdge * pcNextEdge,
	const MbSurface * pcBaseSurface, double dTolerance, bool & bConnectFlag, const MbCurve ** pcConectiCurve)
{
	const MbCurveEdge & cCurCurveEdge = pcCurEdge->GetCurveEdge();
	const MbCurveEdge & cNextCurveEdge = pcNextEdge->GetCurveEdge();

	const MbSurfaceIntersectionCurve & cCurIntersectionCurve = cCurCurveEdge.GetIntersectionCurve();
	const MbSurfaceIntersectionCurve & cNextIntersectionCurve = cNextCurveEdge.GetIntersectionCurve();

	bool bCurSense = pcCurEdge->GetOrientation();
	bool bNextSense = pcNextEdge->GetOrientation();

	const MbCurve * pcCurCurve[2];
	const MbCurve * pcNextCurve[2];

	pcCurCurve[0] = nullptr;
	pcCurCurve[1] = nullptr;

	pcNextCurve[0] = nullptr;
	pcNextCurve[1] = nullptr;

	// Seam Edge의 경우 Curve의 Base Surface가 같음.
	if(pcBaseSurface == cCurIntersectionCurve.GetSurfaceOne()) {
		pcCurCurve[0] = &cCurIntersectionCurve.GetCurveOneCurve();
	}
	if(pcBaseSurface == cCurIntersectionCurve.GetSurfaceTwo()) {
		pcCurCurve[1] = &cCurIntersectionCurve.GetCurveTwoCurve();
	}

	if(pcBaseSurface == cNextIntersectionCurve.GetSurfaceOne()) {
		pcNextCurve[0] = &cNextIntersectionCurve.GetCurveOneCurve();
	}
	if(pcBaseSurface == cNextIntersectionCurve.GetSurfaceTwo()) {
		pcNextCurve[1] = &cNextIntersectionCurve.GetCurveTwoCurve();
	}

	MbCartPoint cCurCurveEdnPoint[2], cNextCurveStartPoint[2];

	if(true == bCurSense) {
		if(nullptr != pcCurCurve[0]) {
			pcCurCurve[0]->GetEndPoint(cCurCurveEdnPoint[0]);
		}
		if(nullptr != pcCurCurve[1]) {
			pcCurCurve[1]->GetEndPoint(cCurCurveEdnPoint[1]);
		}
	}
	else {
		if(nullptr != pcCurCurve[0]) {
			pcCurCurve[0]->GetStartPoint(cCurCurveEdnPoint[0]);
		}
		if(nullptr != pcCurCurve[1]) {
			pcCurCurve[1]->GetStartPoint(cCurCurveEdnPoint[1]);
		}
	}

	if(true == bNextSense) {
		if(nullptr != pcNextCurve[0]) {
			pcNextCurve[0]->GetStartPoint(cNextCurveStartPoint[0]);
		}
		if(nullptr != pcNextCurve[1]) {
			pcNextCurve[1]->GetStartPoint(cNextCurveStartPoint[1]);
		}
	}
	else {
		if(nullptr != pcNextCurve[0]) {
			pcNextCurve[0]->GetEndPoint(cNextCurveStartPoint[0]);
		}
		if(nullptr != pcNextCurve[1]) {
			pcNextCurve[1]->GetEndPoint(cNextCurveStartPoint[1]);
		}
	}

	double dDistnace = DBL_MAX;
	bConnectFlag = false;

	// 사전에 시작점 끝점은 Curve 방향에 따라서 입력되어 있음.
	for(int nCurCurveIndex = 0; nCurCurveIndex < 2; nCurCurveIndex++) {
		for(int nNextCurveIndex = 0; nNextCurveIndex < 2; nNextCurveIndex++) {
			if(nullptr != pcCurCurve[nCurCurveIndex] && nullptr != pcNextCurve[nNextCurveIndex]) {
				dDistnace = cCurCurveEdnPoint[nCurCurveIndex].DistanceToPoint(cNextCurveStartPoint[nNextCurveIndex]);
				if(dDistnace < dTolerance) {
					if(nullptr != pcConectiCurve) {
						pcConectiCurve[0] = pcCurCurve[nCurCurveIndex];
						pcConectiCurve[1] = pcNextCurve[nNextCurveIndex];
					}
					bConnectFlag = true;
					return true;
				}
			}
		}
	}

	return true;
}

// 주어진 두 점을 이용해서 Line Segment를 Base로하는 Oriented Edge를 생성.
// 주어진 Surface를 One, Two로 하는 Oriented Edge를 생성함. 
// 주어진 Oriented Edge는 전후 관계를 확인해서 연결되어 있는 Curve의 값을 사용해야 한다.
bool ConvLoop::CreateLineSegmentOrientedEdge(OrientedEdgeList & lpcOrientedEdgeList,
	const MbOrientedEdge * pcCurEdge, const MbOrientedEdge * pcNextEdge, const MbSurface * pcBaseSurface, double dTolerance,
	const MbOrientedEdge *& pcLineSegmentOrientedEdge)
{
	auto cEndIterator = std::prev(lpcOrientedEdgeList.end());

	auto pcCurEdgeIterator = std::find(lpcOrientedEdgeList.begin(), lpcOrientedEdgeList.end(), pcCurEdge);
	auto pcNextEdgeIterator = std::find(lpcOrientedEdgeList.begin(), lpcOrientedEdgeList.end(), pcNextEdge);

	// 각각의 Edge의 앞쪽 및 뒷쪽 Edge와 비교해서, 연결된 Curve를 이용해서 
	auto pcCurEdgePrevIterator = pcCurEdgeIterator;
	if(pcCurEdgePrevIterator == lpcOrientedEdgeList.begin()) {
		pcCurEdgePrevIterator = lpcOrientedEdgeList.end();
		pcCurEdgePrevIterator--;
	}
	else {
		pcCurEdgePrevIterator--;
	}

	auto pcNextEdgeNextIterator = pcNextEdgeIterator;
	if(pcNextEdgeNextIterator == cEndIterator) {
		pcNextEdgeNextIterator = lpcOrientedEdgeList.begin();
	}
	else {
		pcNextEdgeNextIterator++;
	}

	bool bConnectFlag = false;
	const MbCurve * pcCurCurve[2];
	pcCurCurve[0] = nullptr;
	pcCurCurve[1] = nullptr;
	// current edge의 연결 curve를 찾아온다. 
	// pcCurCurve에 각각 Cur, Next의 연결된 MbCurve를 찾아 온다.
	CheckConectivity(*pcCurEdgePrevIterator, *pcCurEdgeIterator, pcBaseSurface, dTolerance, bConnectFlag, pcCurCurve);

	const MbCurve * pcNextCurve[2];
	pcNextCurve[0] = nullptr;
	pcNextCurve[1] = nullptr;
	// next edge의 연결 curve를 찾아온다. 
	CheckConectivity(*pcNextEdgeIterator, *pcNextEdgeNextIterator, pcBaseSurface, dTolerance, bConnectFlag, pcNextCurve);

	// CurEdgePrev와 CurEdge가 연결된 Curve에서 pcCurCurve[1]가 CureEdge의 curve임.
	// NextEdgeNext와 NextEdge가 연결된 Curve에서 pcNextCurve[0]가 NextEdge의 curve임.
	if(nullptr == pcCurCurve[1] || nullptr == pcNextCurve[0]) {
		return false;
	}

	MbePlaneType eType1 = pcCurCurve[1]->IsA();
	MbePlaneType eType2 = pcNextCurve[0]->IsA();

	bool bCurSense = pcCurEdge->GetOrientation();
	bool bNextSense = pcNextEdge->GetOrientation();

	MbCartPoint cStartPoint, cEndPoint;

	if(true == bCurSense) {
		pcCurCurve[1]->GetEndPoint(cStartPoint);
	}
	else {
		pcCurCurve[1]->GetStartPoint(cStartPoint);
	}

	if(true == bNextSense) {
		pcNextCurve[0]->GetStartPoint(cEndPoint);
	}
	else {
		pcNextCurve[0]->GetEndPoint(cEndPoint);
	}

	MbLineSegment * pcLine1 = new MbLineSegment(cStartPoint, cEndPoint);
	if(nullptr == pcLine1) {
		ASSERT(NULL);
		return false;
	}

	MbLineSegment * pcLine2 = new MbLineSegment(cStartPoint, cEndPoint);
	if(nullptr == pcLine2) {
		ASSERT(NULL);
		return false;
	}

	MbSurfaceIntersectionCurve * pcInterCurve = new MbSurfaceIntersectionCurve(*pcBaseSurface, *pcLine1, *pcBaseSurface, *pcLine2,
		MbeCurveBuildType::cbt_Ordinary, true, false);

	const MbVertex & cStartVertex = pcCurEdge->GetEndVertex();
	const MbVertex & cEndVertex = pcNextEdge->GetBegVertex();

	MbCurveEdge * pcCurveEdge = new MbCurveEdge(cStartVertex, cEndVertex, *pcInterCurve, true);

	pcLineSegmentOrientedEdge = new MbOrientedEdge(*pcCurveEdge, true);

	return true;
}

// OrientedEdge Plane curve가 Line인지 여부를 확인
bool ConvLoop::IsOrientedEdgeAllPlaneLine(std::list<const MbOrientedEdge *> & lpcOrientedEdgeList, const MbSurface * pcBaseSurface)
{
	for(auto pcOrientEdge : lpcOrientedEdgeList) {
		const MbCurveEdge & cCurveEdge = pcOrientEdge->GetCurveEdge();
		const MbSurfaceIntersectionCurve & cIntersectionCurve = cCurveEdge.GetIntersectionCurve();

		if(pcBaseSurface == &cIntersectionCurve.GetCurveOneSurface()) {
			if(pt_LineSegment != cIntersectionCurve.GetCurveOneCurve().IsA()) {
				return false;
			}
		}
		else if(pcBaseSurface == &cIntersectionCurve.GetCurveTwoSurface()) {
			if(pt_LineSegment != cIntersectionCurve.GetCurveTwoCurve().IsA()) {
				return false;
			}
		}
	}

	return true;
}
