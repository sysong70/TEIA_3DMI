#include "stdafx.h"
#include "ConvFace.h"

#include "ConvSurface.h"
#include "ConvLoop.h"

#include "../Surface/DmiSurface.h"

#include <surf_plane.h>
#include <surf_torus_surface.h>
#include <surf_curve_bounded_surface.h>

#include <cur_contour.h>

using namespace c3d;

#define CHECK_BOOL_RETURN(FunctionCall) { bool nResult = FunctionCall; if (nResult == false) { return false; } }
#define CHECK_A3D_RESULT(FunctionCall) { A3DStatus eResult = FunctionCall; if (eResult != A3D_SUCCESS) { return false; } }

// dContextScale은 Tolerance 계산 때문에 필요함.
ConvFace::ConvFace(A3DTopoFace * pcTopoFace, bool bOrientationWithShell, double dContextScale)
{
	m_pcTopoFace = pcTopoFace;

	m_bOrientationWithShell = bOrientationWithShell;

	// Topo Face Data 저장 (Face의 아래 부분에 있는 Data를 저장한다.)
	A3D_INITIALIZE_DATA(A3DTopoFaceData, m_cTopoFaceData);
	A3DStatus eResult = A3DTopoFaceGet(pcTopoFace, &m_cTopoFaceData);
	if(A3D_SUCCESS != eResult) {
		return;
	}

	A3DSurfBase * pcSurface = m_cTopoFaceData.m_pSurface;
	for(A3DUns32 nIndex = 0; nIndex < m_cTopoFaceData.m_uiLoopSize; nIndex++) {
		if(false == GetLoopData(m_cTopoFaceData.m_ppLoops[nIndex], pcSurface, dContextScale)) {
			LogManager::Log(2, L"Error - ConvFace/GetLoopData");
			return;
		}
	}

	m_bInitFlag = true;
}

ConvFace::ConvFace(A3DTopoFace * pcTopoFace, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap & mpcConvSurfaceMap)
{
/*
	m_pcTopoFace = pcTopoFace;

	// Topo Face Data 저장 (Face의 아래 부분에 있는 Data를 저장한다.)
	A3D_INITIALIZE_DATA(A3DTopoFaceData, m_cTopoFaceData);
	A3DStatus eResult = A3DTopoFaceGet(pcTopoFace, &m_cTopoFaceData);
	if(A3D_SUCCESS != eResult) {
		return;
	}

	// Surface 정보 수집
	if(false == ConvObject::GetConvSurfaceData(m_cTopoFaceData.m_pSurface, dContextScale, mpcConvSurfaceMap, m_pcConvSurface)) {
		A3DTopoFaceGet(nullptr, &m_cTopoFaceData);
		return;
	}

	for(A3DUns32 nIndex = 0; nIndex < m_cTopoFaceData.m_uiLoopSize; nIndex++) {
		if(false == GetLoopData(m_cTopoFaceData.m_ppLoops[nIndex], dContextScale, pcBrepData, mpcConvSurfaceMap)) {
			LogManager::Log(2, L"Error - ConvFace/GetLoopData");
			return;
		}
	}
*/

	m_bInitFlag = true;
}

ConvFace::~ConvFace()
{
	if(true == m_bInitFlag) {
		A3DTopoFaceGet(nullptr, &m_cTopoFaceData);
	}
	// Surface는 외부에서 삭제하도록 한다.
}

// == 1. Convert 관련 함수 ===========================================================================
bool ConvFace::Convert(const ConvSurfaceMap & mpcConvSurfaceAtlMap)
{
	// 1. Surace 정보 확인
	if(nullptr == m_pcConvSurface) {
		mpcConvSurfaceAtlMap.Lookup((DWORD_PTR) m_cTopoFaceData.m_pSurface, m_pcConvSurface);
		if(nullptr == m_pcConvSurface) {
			ASSERT(false);
			return false;
		}
	}

	// 2. Loop 정보 Convert
	if(false == ConvertLoop()) {
		ASSERT(false);
	}

	SurfaceSPtr & pcSurface = m_pcConvSurface->GetSurface();
	MbeSpaceType eType = pcSurface->IsA();

	MbSurface * pcOriginSurface = pcSurface;

	bool bOrientationWithShell = m_bOrientationWithShell;

	// 3. 최외각 Loop를 이용해서, Surface의 Boundary를 Curve가 넘어가는지 여부를 확인하고 그런경우 Boundary Surface를 생성해서 사용하도록 한다.
	SurfaceSPtr pcBoundedSurface;
	if(true == CreateBoundedSurface(pcSurface, m_vpcLoopVector[0], bOrientationWithShell, pcBoundedSurface)) {
		pcSurface = pcBoundedSurface;
	}

	// 4. 생성된 Surface를 이용해서 Face를 생성한다.
	switch(pcOriginSurface->IsA())
	{
		// 3-1. Periodic Surface만 적용하도록 한다.
		case st_CylinderSurface:
		case st_ConeSurface:
		case st_SphereSurface:
		case st_TorusSurface:
		{
			const DmiSurface * pcRtSurface = dynamic_cast<const DmiSurface *>(pcOriginSurface);
			if(0.0 > pcRtSurface->GetVCoeff()) {
				bOrientationWithShell = !bOrientationWithShell;
			}
			else if(st_TorusSurface == pcOriginSurface->IsA())
			{
				// V 경계면을 넘어가는 UV 커브가 있는 경우 Major Radius를 Minus로 설정한다.
				if(0.0 > ((MbTorusSurface *) pcOriginSurface)->GetMajorRadius()) {
					bOrientationWithShell = !bOrientationWithShell;
				}
			}
		}
		break;
	}

	// 5. Face 생성
	m_pcFace = new MbFace(*pcSurface, bOrientationWithShell);
	if(nullptr == m_pcFace) {
		ASSERT(false);
		return false;
	}

	// 6. Loop를 Face에 추가.
	for(MbLoop * pcLoopElement : m_vpcLoopVector) {
		m_pcFace->AddLoop(*pcLoopElement);
	}

	return true;
}

bool ConvFace::ConvertLoop()
{
	bool bFirstLoopFlag = true;
	for(auto pcConvLoop : m_vpcConvLoopVector) {
		if(false == pcConvLoop->Convert(m_pcConvSurface, m_bOrientationWithShell, bFirstLoopFlag)) {
			ASSERT(false);
		}

		bFirstLoopFlag = false;
	}

	for(auto pcConvLoop : m_vpcConvLoopVector) {
		m_vpcLoopVector.push_back(pcConvLoop->GetMbLoop());
	}

	return true;
}

// 2. Loop Data 수집
bool ConvFace::GetLoopData(A3DTopoLoop * pcTopoLoop, A3DSurfBase * pcSurfBase, double dContextScale)
{
	ConvLoop * pcLoop = new ConvLoop(pcTopoLoop, pcSurfBase, dContextScale);
	if(nullptr == pcLoop) {
		return false;
	}

	if(false == pcLoop->IsInit()) {
		return false;
	}

	m_vpcConvLoopVector.push_back(pcLoop);

	return true;
}

bool ConvFace::GetLoopData(A3DTopoLoop * pcTopoLoop, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap mpcConvSurfaceMap)
{
	ConvLoop * pcLoop = new ConvLoop(pcTopoLoop, dContextScale, pcBrepData, mpcConvSurfaceMap);
	if(nullptr == pcLoop) {
		return false;
	}

	if(false == pcLoop->IsInit()) {
		return false;
	}

	m_vpcConvLoopVector.push_back(pcLoop);

	return true;
}

// 3. Convert Surface 
bool ConvFace::GetConvSurfaceData(const A3DSurfBase * pcSurfBase, double dContextScale, ConvSurface *& pcSurface)
{
	pcSurface = new ConvSurface(pcSurfBase, dContextScale);
	if(nullptr == pcSurface) {
		return false;
	}

	if(false == pcSurface->IsInit()) {
		return false;
	}

	return true;
}

// 4. Surface 정보 수집
void ConvFace::CollectConvSurface(ConvSurfaceMap & pcSurfBaseMap)
{
	A3DSurfBase * pcSurfBase = m_cTopoFaceData.m_pSurface;

	ConvSurface * pcConvSurface = nullptr;
	if(false == pcSurfBaseMap.Lookup((DWORD_PTR) pcSurfBase, pcConvSurface)) {
		pcSurfBaseMap.SetAt((DWORD_PTR) pcSurfBase, nullptr);
	}
}

// 5. ConvCoEdge 정보 수집
void ConvFace::CollectConvCoEdge(ConvCoEdgeMap & mpcConvCoEdgeAtlMap)
{
	for(auto pcLoop : m_vpcConvLoopVector) {
		pcLoop->CollectConvCoEdge(mpcConvCoEdgeAtlMap);
	}
}

// 5. TopoVertex 정보 수집
void ConvFace::CollectTopoVertex(ConvVertexMap & mpcConvVertexAtlMap)
{
	for(auto pcLoop : m_vpcConvLoopVector) {
		pcLoop->CollectTopoVertex(mpcConvVertexAtlMap);
	}
}

// == 6. Utility 함수 ===============================================================================

// 6-1. Loop의 Parameter 영역이 Surface의 영역을 벗어난 경우 Surface를 바탕으로 MbCurveBoundedSurface를 생성한다.
bool ConvFace::CreateBoundedSurface(SurfaceSPtr & pcSurface, LoopSPtr & pcLoop, bool bOrientationWithShell, SurfaceSPtr & pcBoundedSurface)
{
	const MbRect cLoopRect = pcLoop->GetGabarit(pcSurface->GetSurface(), bOrientationWithShell);
	if(true == cLoopRect.IsEmpty()) {
		ASSERT(false);
		return false;
	}

	PlaneContourSPtr pcLoopContour(&pcLoop->MakeContour(*pcSurface, bOrientationWithShell));

	PlaneContoursSPtrVector pcloopContoursVector;
	pcloopContoursVector.push_back(pcLoopContour);

	pcBoundedSurface = new MbCurveBoundedSurface(*pcSurface, pcloopContoursVector, false);

	return true;
/*
	// 공차를 주어야 한다. Loop의 Bound Box가 약간이라도 벗어나면 문제가 생김.
	double dTolerance = 1.0e-2;

	bool bCreateBoundedSurface = false;

	double dUMin = pcSurface->GetUMin();
	double dUMax = pcSurface->GetUMax();
	double dVMin = pcSurface->GetVMin();
	double dVMax = pcSurface->GetVMax();

	if(cLoopRect.GetXMin() - dTolerance < dUMin) {
		bCreateBoundedSurface = true;
	}
	else if(cLoopRect.GetXMax() + dTolerance > dUMax) {
		bCreateBoundedSurface = true;
	}
	else if(cLoopRect.GetYMin() - dTolerance < dVMin) {
		bCreateBoundedSurface = true;
	}
	else if(cLoopRect.GetYMax() + dTolerance > dVMax) {
		bCreateBoundedSurface = true;
	}

	//bCreateBoundedSurface = true;

	if(false == bCreateBoundedSurface) {
		return false;
	}

	PlaneContourSPtr pcLoopContour(&pcLoop->MakeContour(*pcSurface, bOrientationWithShell));

	PlaneContoursSPtrVector pcloopContoursVector;
	pcloopContoursVector.push_back(pcLoopContour);

	pcBoundedSurface = new MbCurveBoundedSurface(*pcSurface, pcloopContoursVector, false);

	return true;
*/
}