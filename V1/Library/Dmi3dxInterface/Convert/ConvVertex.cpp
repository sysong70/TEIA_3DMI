#include "stdafx.h"
#include "ConvVertex.h"

#include <topology.h>

using namespace c3d;

#define CHECK_BOOL_RETURN(FunctionCall) { bool nResult = FunctionCall; if (nResult == false) { return false; } }
#define CHECK_A3D_RESULT(FunctionCall) { A3DStatus eResult = FunctionCall; if (eResult != A3D_SUCCESS) { return false; } }

ConvVertex::ConvVertex(A3DTopoVertex * pcTopoVertex)
{
	A3DStatus eResult = A3DEntityGetType(pcTopoVertex, &m_eVertexType);
	if(A3D_SUCCESS != eResult) {
		LogManager::Log(2, L"Error - ConvVertex/A3DEntityGetType [%d]", eResult);
		return;
	}

	m_pcTopoVertex = pcTopoVertex;

	bool bStatus = false;
	switch(m_eVertexType)
	{
		case kA3DTypeTopoUniqueVertex:
			bStatus = GetTopoUniqueVertex(pcTopoVertex);
			break;

		case kA3DTypeTopoMultipleVertex:
			bStatus = GetTopoMultipleVertex(pcTopoVertex);
			break;

		default:
			LogManager::Log(2, L"Error - ConvVertex/Vertex type unknown [%d]", m_eVertexType);
			return;
			break;
	}

	m_bInitFlag = true;
}

ConvVertex::~ConvVertex()
{
	if(true == m_bInitFlag && nullptr != m_pcVertexData) {

		switch(m_eVertexType)
		{
			case kA3DTypeTopoUniqueVertex:
				A3DTopoUniqueVertexGet(nullptr, (A3DTopoUniqueVertexData *) m_pcVertexData);
				break;

			case kA3DTypeTopoMultipleVertex:
				A3DTopoMultipleVertexGet(nullptr, (A3DTopoMultipleVertexData *) m_pcVertexData);
				break;
		}
	}

	if(nullptr != m_pcVertexData) {
		delete m_pcVertexData;;
	}
}
// == 변환 관련 함수 =============================================================================

bool ConvVertex::Convert(double dContextScale)
{
	return ConvertTopoVertex(dContextScale, m_pcVertex);
}

bool ConvVertex::ConvertTopoVertex(double dContextScale, VertexSPtr & pcVertex)
{
	if(nullptr == m_pcVertexData || kA3DTypeUnknown == m_eVertexType) {
		return false;
	}

	bool bStatus = false;
	switch(m_eVertexType)
	{
		case kA3DTypeTopoUniqueVertex:
			bStatus = ConvertTopoUniqueVertex(dContextScale, pcVertex);
			break;

		case kA3DTypeTopoMultipleVertex:
			bStatus = ConvertTopoMultipleVertex(dContextScale, pcVertex);
			break;
	}

	return bStatus;
}

bool ConvVertex::ConvertTopoUniqueVertex(double dContextScale, VertexSPtr & pcVertex)
{
	A3DTopoUniqueVertexData & cTopoUniqueVertexData = *((A3DTopoUniqueVertexData *) m_pcVertexData);

	MbCartPoint3D cCartPoint3d;
	cCartPoint3d.x = cTopoUniqueVertexData.m_sPoint.m_dX * dContextScale;
	cCartPoint3d.y = cTopoUniqueVertexData.m_sPoint.m_dY * dContextScale;
	cCartPoint3d.z = cTopoUniqueVertexData.m_sPoint.m_dZ * dContextScale;

	pcVertex = new MbVertex(cCartPoint3d);
	if(nullptr == pcVertex) {
		return false;
	}

	return true;
}

bool ConvVertex::ConvertTopoMultipleVertex(double dContextScale, VertexSPtr & pcVertex)
{
	A3DTopoMultipleVertexData & cMultipleVertexData = *((A3DTopoMultipleVertexData *) m_pcVertexData);

	if(0 == cMultipleVertexData.m_uiSize) {
		return false;
	}

	MbCartPoint3D cCartPoint3d;
	cCartPoint3d.x = cMultipleVertexData.m_pPts[0].m_dX * dContextScale;
	cCartPoint3d.y = cMultipleVertexData.m_pPts[0].m_dY * dContextScale;
	cCartPoint3d.z = cMultipleVertexData.m_pPts[0].m_dZ * dContextScale;

	pcVertex = new MbVertex(cCartPoint3d);
	if(nullptr == pcVertex) {
		return false;
	}

	return true;
}

// == Vertex 관련 함수 ==============================================================================

// 1. Unique Vertex Data 수집
bool ConvVertex::GetTopoUniqueVertex(A3DTopoVertex * pcTopoVertex)
{
	m_pcVertexData = (DWORD_PTR *)new A3DTopoUniqueVertexData();
	A3D_INITIALIZE_DATA(A3DTopoUniqueVertexData, (*((A3DTopoUniqueVertexData *) m_pcVertexData)));
	if(A3D_SUCCESS != A3DTopoUniqueVertexGet(pcTopoVertex, (A3DTopoUniqueVertexData *) m_pcVertexData)) {
		return false;
	}

	return true;
}

// 2. Multiple Vertex Data 수집
bool ConvVertex::GetTopoMultipleVertex(A3DTopoVertex * pcTopoVertex)
{
	m_pcVertexData = (DWORD_PTR *)new A3DTopoMultipleVertexData();
	A3D_INITIALIZE_DATA(A3DTopoMultipleVertexData, (*((A3DTopoMultipleVertexData *) m_pcVertexData)));
	if(A3D_SUCCESS != A3DTopoMultipleVertexGet(pcTopoVertex, (A3DTopoMultipleVertexData *) m_pcVertexData)) {
		return false;
	}

	return true;
}