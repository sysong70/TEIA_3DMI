#include "stdafx.h"

#include "ConvFaceShell.h"

#include "ConvFace.h"
#include "ConvSurface.h"
#include "ConvCoEdge.h"
#include "ConvEdge.h"
#include "ConvVertex.h"

#include <topology.h>

#include <surface.h>

#include <ppl.h>

using namespace c3d;
using namespace concurrency;

#define CHECK_BOOL_RETURN(FunctionCall) { bool nResult = FunctionCall; if (nResult == false) { return false; } }
#define CHECK_A3D_RESULT(FunctionCall) { A3DStatus eResult = FunctionCall; if (eResult != A3D_SUCCESS) { return false; } }

ConvFaceShell::ConvFaceShell(A3DTopoShell * pcTopoShell, double dContextScale)
{
	A3D_INITIALIZE_DATA(A3DTopoShellData, m_cTopoShellData);
	A3DStatus nResult = A3DTopoShellGet(pcTopoShell, &m_cTopoShellData);
	if(A3D_SUCCESS != nResult) {
		ASSERT(false);
		return;
	}

	for(A3DUns32 nIndex = 0; nIndex < m_cTopoShellData.m_uiFaceSize; nIndex++)
	{
		bool bOrientationWithShell = (1 == m_cTopoShellData.m_pucOrientationWithShell[nIndex]) ? true : false;
		ConvFace * pcConvFace = new ConvFace(m_cTopoShellData.m_ppFaces[nIndex], bOrientationWithShell, dContextScale);
		m_vpcConvFaceVector.push_back(pcConvFace);
	}

	m_bInitFlag = true;
}

ConvFaceShell::~ConvFaceShell()
{
	if(true == m_bInitFlag) {
		A3DTopoShellGet(nullptr, &m_cTopoShellData);
	}
	// Surface는 외부에서 삭제하도록 한다.
}

// 1. ConvFaceShell에 들어있는 Surface를 검색해서 변환한다.
bool ConvFaceShell::CollectAndConvertSurface(double dContextScale)
{
	// 1. Surface 정보 수집 (A3DSurfBase 정보만 수집)
	for(auto pcConFace : m_vpcConvFaceVector) {
		pcConFace->CollectConvSurface(m_mpcConvSurfaceMap);
	}

	if(true == m_mpcConvSurfaceMap.IsEmpty()) {
		return false;
	}

	// 2. 수집된 A3DSurfBase를 이용해서 ConvSurface를 생성 (ConvSurface를 생성할 때 까지는 3DX DB를 사용하기 때문에 Thread를 돌리면 않됨)
	POSITION pcPosition = m_mpcConvSurfaceMap.GetStartPosition();
	int nCount = (int) m_mpcConvSurfaceMap.GetCount();

	for(int nIndex = 0; nIndex < nCount; nIndex++) {
		const A3DSurfBase * pcSurfBase = (const A3DSurfBase *) m_mpcConvSurfaceMap.GetNextKey(pcPosition);
		ConvSurface * pcConvSurface = new ConvSurface(pcSurfBase, dContextScale);
		if(nullptr == pcConvSurface) {
			ASSERT(false);
			continue;
		}

		if(false == pcConvSurface->IsInit()) {
			ASSERT(false);
			continue;
		}

		m_mpcConvSurfaceMap.SetAt((DWORD_PTR) pcSurfBase, pcConvSurface);
	}

	pcPosition = m_mpcConvSurfaceMap.GetStartPosition();

	// #OpenMP
//#pragma omp parallel for
	for(int nIndex = 0; nIndex < nCount; nIndex++) 
	{
		ConvSurface * pcConvSurface = m_mpcConvSurfaceMap.GetNextValue(pcPosition);
		bool bStatus = pcConvSurface->ConvertSurface(dContextScale);
		if(false == bStatus) {
			ASSERT(false);
		}

		if(nullptr == pcConvSurface->GetSurface()) {
			ASSERT(false);
		}
	}

	return true;
}

bool ConvFaceShell::CollectAndConvertSurfaceMap(double dContextScale)
{
	/*// 1. Surface 정보 수집 (A3DSurfBase 정보만 수집)
	for(auto pcConFace : m_vpcConvFaceVector) {
		pcConFace->CollectConvSurface(m_mpcConvSurfaceMap);
	}

	if(true == m_mpcConvSurfaceMap.empty()) {
		return false;
	}

	// 2. 수집된 A3DSurfBase를 이용해서 ConvSurface를 생성 (ConvSurface를 생성할 때 까지는 3DX DB를 사용하기 때문에 Thread를 돌리면 않됨)
	for(auto & cIterator : m_mpcConvSurfaceMap) {
		const A3DSurfBase * pcSurfBase = cIterator.first;
		cIterator.second = new ConvSurface(pcSurfBase, dContextScale);
	}

	//parallel_for_each(m_mpcConvSurfaceMap.begin(), m_mpcConvSurfaceMap.end(), [dContextScale] (auto & cIterator) {
	for_each(m_mpcConvSurfaceMap.begin(), m_mpcConvSurfaceMap.end(), [dContextScale] (auto & cIterator) {
		ConvSurface * pcConvSurface = cIterator.second;
		bool bStatus = pcConvSurface->ConvertSurface(dContextScale);
		if(false == bStatus) {
			ASSERT(false);
		}
	});*/

	return true;
}

// 2. ConvCoEdge를 수집해서, MbCurveEdge를 생성하도록 한다.
bool ConvFaceShell::CollectAndConvertTopoCoEdge(double dContextScale)
{
	// ConvCoEdge 정보 수집
	for(auto pcConFace : m_vpcConvFaceVector) {
		pcConFace->CollectConvCoEdge(m_mpcConvCoEdgeMap);
	}

	// ConvCoEdge에 Suface 활당
	POSITION pcPosition = m_mpcConvCoEdgeMap.GetStartPosition();
	while(nullptr != pcPosition) {
		ConvCoEdge * pcConvCoEdge = m_mpcConvCoEdgeMap.GetNextValue(pcPosition);
		if(nullptr == pcConvCoEdge) {
			ASSERT(false);
			continue;
		}

		ConvSurface * pcConvSurface = nullptr;
		if(true == m_mpcConvSurfaceMap.Lookup((DWORD_PTR) pcConvCoEdge->GetSurfBase(), pcConvSurface)) {
			if(nullptr == pcConvSurface->GetSurface()) {
				ASSERT(false);
			}
			pcConvCoEdge->SetSurface(pcConvSurface->GetSurface());
		}
		else {
			ASSERT(false);
		}
	}

	pcPosition = m_mpcConvCoEdgeMap.GetStartPosition();
	int nCount = (int) m_mpcConvCoEdgeMap.GetCount();

	// #OpenMP
//#pragma omp parallel for
	for(int nIndex = 0; nIndex < nCount; nIndex++) {
		ConvCoEdge * pcConvCoEdge = m_mpcConvCoEdgeMap.GetNextValue(pcPosition);
		bool bStatus = pcConvCoEdge->ConvertCurve(dContextScale);
		if(false == bStatus) {
			ASSERT(false);
		}
	}

	return true;
}

bool ConvFaceShell::CollectAndConvertTopoCoEdgeMap(double dContextScale)
{
	/*// ConvCoEdge 정보 수집
	for(auto pcConFace : m_vpcConvFaceVector) {
		pcConFace->CollectConvCoEdge(m_mpcConvCoEdgeMap);
	}

	// ConvCoEdge에 Suface와 이웃 ConvCoedge 활당
	for(auto & cIterator : m_mpcConvCoEdgeMap) {
		ConvCoEdge * pcCoEdge = cIterator.second;

		if(nullptr != pcCoEdge->GetNeighborTopoCoEdge()) {
			auto pcNeighborConvCoEdge = m_mpcConvCoEdgeMap.find(pcCoEdge->GetNeighborTopoCoEdge());
			if(pcNeighborConvCoEdge != m_mpcConvCoEdgeMap.end()) {
				pcCoEdge->SetNeighborConvCoEdge(pcCoEdge);
			}
			else {
				ASSERT(false);
			}
		}

		// CoEdge의 SurfBase를 이용해서 ConvSurface를 찾는다.
		auto pcFindIterator = m_mpcConvSurfaceMap.find(pcCoEdge->GetSurfBase());
		if(pcFindIterator != m_mpcConvSurfaceMap.end()) {
			ConvSurface * pcConvSurface = pcFindIterator->second;
			c3d::SurfaceSPtr pcSurface = pcConvSurface->GetSurface();
			pcCoEdge->SetSurface(pcSurface);
		}
		else {
			ASSERT(false);
		}
	}

	parallel_for_each(m_mpcConvCoEdgeMap.begin(), m_mpcConvCoEdgeMap.end(), [dContextScale] (auto & cIterator) {
		ConvCoEdge & cCoEdge = *cIterator.second;
		if(false == cCoEdge.ConvertCurve(dContextScale)) {
			ASSERT(false);
		}
	});*/

	return true;
}

// 3. TopoVertex를 수집해서, MbVertex를 생성하도록 한다.
bool ConvFaceShell::CollectAndConvertTopoVertex(double dContextScale)
{
	// 1. TopoVertex 정보 수집
	for(auto pcConFace : m_vpcConvFaceVector) {
		pcConFace->CollectTopoVertex(m_mpcConvVertexMap);
	}

	// 2. TopoVertex와 연결되는 ConvVertex 생성
	POSITION pcPosition = m_mpcConvVertexMap.GetStartPosition();
	while(nullptr != pcPosition) {
		A3DTopoVertex * pcTopoVertex = (A3DTopoVertex *) m_mpcConvVertexMap.GetNextKey(pcPosition);
		m_mpcConvVertexMap.SetAt((DWORD_PTR) pcTopoVertex, new ConvVertex(pcTopoVertex));
	}

	pcPosition = m_mpcConvVertexMap.GetStartPosition();
	int nCount = (int) m_mpcConvVertexMap.GetCount();

	DWORD_PTR * pnConvVertexArray = new DWORD_PTR[nCount];

	for(int nIndex = 0; nIndex < nCount; nIndex++) {
		pnConvVertexArray[nIndex] = (DWORD_PTR) m_mpcConvVertexMap.GetNextValue(pcPosition);
	}

	// #OpenMP
//#pragma omp parallel for
	for(int nIndex = 0; nIndex < nCount; nIndex++) {
		ConvVertex * pcConvVertex = (ConvVertex *) pnConvVertexArray[nIndex];
		bool bStatus = pcConvVertex->Convert(dContextScale);
		if(false == bStatus) {
			ASSERT(false);
		}
	}

	delete[]pnConvVertexArray;

	pcPosition = m_mpcConvCoEdgeMap.GetStartPosition();
	while(nullptr != pcPosition) {
		ConvCoEdge * pcConvCoEdge = (ConvCoEdge *) m_mpcConvCoEdgeMap.GetNextValue(pcPosition);
		ConvEdge * pcConvEdge = pcConvCoEdge->GetConvEdge();

		A3DTopoVertex * pcStartTopoVertex = pcConvEdge->GetStartTopoVertex();
		A3DTopoVertex * pcEndTopoVertex = pcConvEdge->GetEndTopoVertex();

		ConvVertex * pcStartConvVertex = nullptr;
		if(true == m_mpcConvVertexMap.Lookup((DWORD_PTR) pcStartTopoVertex, pcStartConvVertex)) {
			pcConvEdge->SetStartConvVertex(pcStartConvVertex);
		}
		else {
			ASSERT(false);
		}

		ConvVertex * pcEndConvVertex = nullptr;
		if(true == m_mpcConvVertexMap.Lookup((DWORD_PTR) pcEndTopoVertex, pcEndConvVertex)) {
			pcConvEdge->SetEndConvVertex(pcEndConvVertex);
		}
		else {
			ASSERT(false);
		}
	}

	return true;
}

bool ConvFaceShell::CollectAndConvertTopoVertexMap(double dContextScale)
{
	// TopoVertex 정보 수집
/*
	for(auto pcConFace : m_vpcConvFaceVector) {
		pcConFace->CollectTopoVertex(m_mpcConvVertexMap);
	}

	for_each(m_mpcConvVertexMap.begin(), m_mpcConvVertexMap.end(), [dContextScale] (auto & cIterator) {
		A3DTopoVertex * pcTopoVertex = cIterator.first;
		cIterator.second = new ConvVertex(pcTopoVertex);
	});

	parallel_for_each(m_mpcConvVertexMap.begin(), m_mpcConvVertexMap.end(), [dContextScale] (auto & cIterator) {
	//for_each(m_mpcConvVertexMap.begin(), m_mpcConvVertexMap.end(), [dContextScale] (auto & cIterator) {
		ConvVertex * pcConvVertex = cIterator.second;
		pcConvVertex->Convert(dContextScale);
	});

	ConvVertexMap & mpcConvVertexMap = m_mpcConvVertexMap;

	for_each(m_mpcConvCoEdgeMap.begin(), m_mpcConvCoEdgeMap.end(), [mpcConvVertexMap] (auto & cIterator) {
		ConvCoEdge * pcConvCoEdge = cIterator.second;
		ConvEdge * pcConvEdge = pcConvCoEdge->GetConvEdge();

		A3DTopoVertex * pcStartTopoVertex = pcConvEdge->GetStartTopoVertex();
		A3DTopoVertex * pcEndTopoVertex = pcConvEdge->GetEndTopoVertex();

		auto pcStartIterator = mpcConvVertexMap.find(pcStartTopoVertex);
		auto pcEndIterator = mpcConvVertexMap.find(pcEndTopoVertex);

		if(pcStartIterator != mpcConvVertexMap.end()) {
			pcConvEdge->SetStartConvVertex(pcStartIterator->second);
		}
		
		if(pcEndIterator != mpcConvVertexMap.end()) {
			pcConvEdge->SetEndConvVertex(pcEndIterator->second);
		}
	});
*/

	return true;
}

// 4. 전체적인 Convert를 실시한다.
bool ConvFaceShell::Convert()
{
	// 1. Face 별로 Convert 실시
	// #OpenMP
#pragma omp parallel for
	for(int nIndex = 0; nIndex < m_vpcConvFaceVector.size(); nIndex++) {
		ConvFace * pcConvFace = m_vpcConvFaceVector[nIndex];
		if(false == pcConvFace->Convert(m_mpcConvSurfaceMap)) {
			ASSERT(false);
		}
	}

	return true;
}

bool ConvFaceShell::ConvertMap()
{
	// 1. Face 별로 Convert 실시
/*
	ConvSurfaceMap & mpcConvSurfaceMap = m_mpcConvSurfaceMap;
	//parallel_for_each(m_vpcConvFaceVector.begin(), m_vpcConvFaceVector.end(), [mpcConvSurfaceMap] (auto pcConvFace) {
	for_each(m_vpcConvFaceVector.begin(), m_vpcConvFaceVector.end(), [mpcConvSurfaceMap] (auto pcConvFace) {
		if(false == pcConvFace->ConvertMap(mpcConvSurfaceMap)) {
			ASSERT(false);
		}
	});
*/

	return true;
}