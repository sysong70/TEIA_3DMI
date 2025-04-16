#include "stdafx.h"
#include "ConvEdge.h"

#include "ConvCurve.h"
#include "ConvVertex.h"

#define CHECK_BOOL_RETURN(FunctionCall) { bool nResult = FunctionCall; if (nResult == false) { return false; } }
#define CHECK_A3D_RESULT(FunctionCall) { A3DStatus eResult = FunctionCall; if (eResult != A3D_SUCCESS) { return false; } }

ConvEdge::ConvEdge(A3DTopoEdge * pcTopoEdge)
{
	// Edge Data 저장
	A3D_INITIALIZE_DATA(A3DTopoEdgeData, m_cTopoEdgeData);
	A3DStatus eResult = A3DTopoEdgeGet(pcTopoEdge, &m_cTopoEdgeData);
	if(A3D_SUCCESS != eResult) {
		LogManager::Log(2, L"Error - ConvEdge/A3DTopoEdgeGet");
		return;
	}

/*
	// ----- Vertex 정보 수집 -----
	if(false == GetVertexData(m_cTopoEdgeData.m_pStartVertex, m_pcStartConvVertex)) {
		ASSERT(false);
		A3DTopoEdgeGet(nullptr, &m_cTopoEdgeData);
		LogManager::Log(2, L"Error - ConvEdge/Start GetVertexData");
		return;
	}

	if(false == GetVertexData(m_cTopoEdgeData.m_pEndVertex, m_pcEndConvVertex)) {
		ASSERT(false);
		A3DTopoEdgeGet(nullptr, &m_cTopoEdgeData);
		LogManager::Log(2, L"Error - ConvEdge/End GetVertexData");
		return;
	}
*/

	m_bInitFlag = true;
}

ConvEdge::~ConvEdge()
{
	if(true == m_bInitFlag) {
		A3DTopoEdgeGet(nullptr, &m_cTopoEdgeData);
	}

	// Vertex 정보 초기화
	if(nullptr != m_pcStartConvVertex) {
		delete m_pcStartConvVertex;
	}
	if(nullptr != m_pcEndConvVertex) {
		delete m_pcEndConvVertex;
	}
}

A3DTopoVertex * ConvEdge::GetStartTopoVertex()
{
	return m_cTopoEdgeData.m_pStartVertex;
}

A3DTopoVertex * ConvEdge::GetEndTopoVertex()
{
	return m_cTopoEdgeData.m_pEndVertex;
}


// 1. Vertext 정보 수집
bool ConvEdge::GetVertexData(A3DTopoVertex * pcTopoVertex, ConvVertex *& pcVertex)
{
	// Catia V4에서는 Vertex가 없는 CoEdge가 나옴.
	if(nullptr == pcTopoVertex) {
		LogManager::Log(2, L"Error - ConvEdge/GetVertexData pcTopoVertex NULL");
		return true;
	}

	pcVertex = new ConvVertex(pcTopoVertex);
	if(nullptr == pcVertex) {
		return false;
	}

	if(false == pcVertex->IsInit()) {
		return false;
	}

	return true;
}

// 2. TopoVertex 정보 수집
void ConvEdge::CollectTopoVertex(ConvVertexMap & mpcConvVertexAtlMap)
{
	if(nullptr != m_cTopoEdgeData.m_pStartVertex) {
		ConvVertex * pcConvVertex = nullptr;
		if(false == mpcConvVertexAtlMap.Lookup((DWORD_PTR) m_cTopoEdgeData.m_pStartVertex, pcConvVertex)) {
			mpcConvVertexAtlMap.SetAt((DWORD_PTR) m_cTopoEdgeData.m_pStartVertex, nullptr);
		}
	}

	if(nullptr != m_cTopoEdgeData.m_pEndVertex) {
		ConvVertex * pcConvVertex = nullptr;
		if(false == mpcConvVertexAtlMap.Lookup((DWORD_PTR) m_cTopoEdgeData.m_pEndVertex, pcConvVertex)) {
			mpcConvVertexAtlMap.SetAt((DWORD_PTR) m_cTopoEdgeData.m_pEndVertex, nullptr);
		}
	}
}