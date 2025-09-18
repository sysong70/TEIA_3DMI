#include <StdAfx.h>

#include "3DX.Simplifier.Impl.h"


//== SimplifierKit Class ==========================================================================
H3DX::SimplifierKitImpl::SimplifierKitImpl()
{

}

//== Simplifier Class =============================================================================
H3DX::SimplifierImpl::SimplifierImpl()
{
}

//== Copy Data Class ==============================================================================
bool H3DX::SimplifierImpl::CopyBrepData(const A3DTopoBrepData * pcInBrepData, A3DTopoBrepData *& pcOutBrepData)
{
	if (nullptr == pcInBrepData) {
		return false;
	}

	A3DTopoBrepDataData cInBrepDataData;
	A3D_INITIALIZE_DATA(A3DTopoBrepDataData, cInBrepDataData);
	A3DStatus nResult = A3DTopoBrepDataGet(pcInBrepData, &cInBrepDataData);

	if (A3D_SUCCESS != nResult) {
		return false;
	}

	// Brep DataData 구조체를 복사하는 부분.
	A3DTopoBrepDataData cCoypBrepDataData;
	CopyBrepDataData(cInBrepDataData, cCoypBrepDataData);

	nResult = A3DTopoBrepDataCreate(&cCoypBrepDataData, &pcOutBrepData);

	if (A3D_SUCCESS != nResult) {
		return false;
	}

	return true;
}

bool H3DX::SimplifierImpl::CopyBrepDataData(const A3DTopoBrepDataData & cInBrepDataData, A3DTopoBrepDataData & cOutBrepDataData)
{
	if (0 == cInBrepDataData.m_uiConnexSize) {
		return false;
	}

	// Data 구조체를 초기화합니다.
	A3D_INITIALIZE_DATA(A3DTopoBrepDataData, cOutBrepDataData);

	// Input BrepDataData에서 ConnexSize를 가져와서 Connex 배열을 할당합니다.
	cOutBrepDataData.m_uiConnexSize = cInBrepDataData.m_uiConnexSize;
	cOutBrepDataData.m_ppConnexes = new A3DTopoConnex * [cOutBrepDataData.m_uiConnexSize];

	for(A3DUns32 nIndex = 0; nIndex < cOutBrepDataData.m_uiConnexSize; nIndex++) {
		if (false == CopyConnex(cInBrepDataData.m_ppConnexes[nIndex], cOutBrepDataData.m_ppConnexes[nIndex])) {
			delete[] cOutBrepDataData.m_ppConnexes;
			return false;
		}
	}
/*
	

	A3DTopoBrepDataData cBrepDataData;
	if (false == GetBrepDataData(pcInBrepData, cBrepDataData)) {
		return false;
	}
	A3DTopoBrepData * pcOutBrepData = nullptr;
	A3DStatus nResult = A3DTopoBrepDataNew(&pcOutBrepData);
	if (A3D_SUCCESS != nResult) {
		return false;
	}
	CopyBrepDataData(cBrepDataData, pcOutBrepData->m_data);*/
	return true;
}

bool H3DX::SimplifierImpl::CopyConnex(const A3DTopoConnex * pcInConnex, A3DTopoConnex *& pcOutCopyConnex)
{
	if (nullptr == pcInConnex) {
		return false;
	}

	A3DTopoConnexData cInConnexData;
	A3D_INITIALIZE_DATA(A3DTopoConnexData, cInConnexData);
	A3DStatus nResult = A3DTopoConnexGet(pcInConnex, &cInConnexData);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	A3DTopoConnexData cOutCopyConnexData;
	A3D_INITIALIZE_DATA(A3DTopoConnexData, cOutCopyConnexData);

	cOutCopyConnexData.m_uiShellSize = cInConnexData.m_uiShellSize;
	cOutCopyConnexData.m_ppShells = new A3DTopoShell * [cOutCopyConnexData.m_uiShellSize];

	for (A3DUns32 nIndex = 0; nIndex < cOutCopyConnexData.m_uiShellSize; nIndex++) {
		if (false == CopyShell(cInConnexData.m_ppShells[nIndex], cOutCopyConnexData.m_ppShells[nIndex])) {
			delete[] cOutCopyConnexData.m_ppShells;
			return false;
		}
	}

	nResult = A3DTopoConnexCreate(&cOutCopyConnexData, &pcOutCopyConnex);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	return true;
}

bool H3DX::SimplifierImpl::CopyShell(const A3DTopoShell * pcInShell, A3DTopoShell *& pcOutCopyShell)
{
	if (nullptr == pcInShell) {
		return false;
	}

	A3DTopoShellData cInShellData;
	A3D_INITIALIZE_DATA(A3DTopoShellData, cInShellData);
	A3DStatus nResult = A3DTopoShellGet(pcInShell, &cInShellData);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	A3DTopoShellData cOutCopyShellData;
	A3D_INITIALIZE_DATA(A3DTopoShellData, cOutCopyShellData);

	cOutCopyShellData.m_bClosed = cInShellData.m_bClosed;
	cOutCopyShellData.m_uiFaceSize = cInShellData.m_uiFaceSize;
	cOutCopyShellData.m_ppFaces = new A3DTopoFace * [cOutCopyShellData.m_uiFaceSize];
	cOutCopyShellData.m_pucOrientationWithShell = new A3DUns8[cOutCopyShellData.m_uiFaceSize];

	for (A3DUns32 nIndex = 0; nIndex < cOutCopyShellData.m_uiFaceSize; nIndex++) {
		cOutCopyShellData.m_pucOrientationWithShell[nIndex] = cInShellData.m_pucOrientationWithShell[nIndex];

		//if (false == CopyFace(cInShellData.m_ppFaces[nIndex], cOutCopyShellData.m_ppFaces[nIndex])) {
		if (false == CopyFaceWithOuterLoopOnly(cInShellData.m_ppFaces[nIndex], cOutCopyShellData.m_ppFaces[nIndex])) {
			delete[] cOutCopyShellData.m_ppFaces;
			delete[] cOutCopyShellData.m_pucOrientationWithShell;
			return false;
		}
	}

	nResult = A3DTopoShellCreate(&cOutCopyShellData, &pcOutCopyShell);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	return true;
}

bool H3DX::SimplifierImpl::CopyFace(const A3DTopoFace * pcInFace, A3DTopoFace *& pcOutCopyFace)
{
	if (nullptr == pcInFace) {
		return false;
	}

	A3DTopoFaceData cInFaceData;
	A3D_INITIALIZE_DATA(A3DTopoFaceData, cInFaceData);
	A3DStatus nResult = A3DTopoFaceGet(pcInFace, &cInFaceData);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	A3DTopoFaceData cOutCopyFaceData;
	A3D_INITIALIZE_DATA(A3DTopoFaceData, cOutCopyFaceData);

	// Face 정보 복사
	cOutCopyFaceData.m_pSurface = cInFaceData.m_pSurface; // 일단 저장되어 있는 Surface를 그대로 사용합니다.
	cOutCopyFaceData.m_bHasTrimDomain = cInFaceData.m_bHasTrimDomain;
	cOutCopyFaceData.m_sSurfaceDomain = cInFaceData.m_sSurfaceDomain;

	cOutCopyFaceData.m_uiLoopSize = cInFaceData.m_uiLoopSize;
	cOutCopyFaceData.m_ppLoops = new A3DTopoLoop * [cOutCopyFaceData.m_uiLoopSize];

	cOutCopyFaceData.m_uiOuterLoopIndex = cInFaceData.m_uiOuterLoopIndex;
	cOutCopyFaceData.m_dTolerance = cInFaceData.m_dTolerance;

	for (A3DUns32 nIndex = 0; nIndex < cOutCopyFaceData.m_uiLoopSize; nIndex++) {
		if (false == CopyLoop(cInFaceData.m_ppLoops[nIndex], cOutCopyFaceData.m_ppLoops[nIndex])) {
			delete[] cOutCopyFaceData.m_ppLoops;
			return false;
		}
	}

	nResult = A3DTopoFaceCreate(&cOutCopyFaceData, &pcOutCopyFace);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	return true;
}

bool H3DX::SimplifierImpl::CopyFaceWithOuterLoopOnly(const A3DTopoFace * pcInFace, A3DTopoFace *& pcOutCopyFace)
{
	if (nullptr == pcInFace) {
		return false;
	}

	A3DTopoFaceData cInFaceData;
	A3D_INITIALIZE_DATA(A3DTopoFaceData, cInFaceData);
	A3DStatus nResult = A3DTopoFaceGet(pcInFace, &cInFaceData);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	A3DTopoFaceData cOutCopyFaceData;
	A3D_INITIALIZE_DATA(A3DTopoFaceData, cOutCopyFaceData);

	// Face 정보 복사
	cOutCopyFaceData.m_pSurface = cInFaceData.m_pSurface; // 일단 저장되어 있는 Surface를 그대로 사용합니다.
	cOutCopyFaceData.m_bHasTrimDomain = cInFaceData.m_bHasTrimDomain;
	cOutCopyFaceData.m_sSurfaceDomain = cInFaceData.m_sSurfaceDomain;

 	cOutCopyFaceData.m_uiLoopSize = 1;
 	cOutCopyFaceData.m_ppLoops = new A3DTopoLoop * [cOutCopyFaceData.m_uiLoopSize];

	cOutCopyFaceData.m_uiOuterLoopIndex = 0;
	cOutCopyFaceData.m_dTolerance = cInFaceData.m_dTolerance;

	if (false == CopyLoop(cInFaceData.m_ppLoops[cInFaceData.m_uiOuterLoopIndex], cOutCopyFaceData.m_ppLoops[0])) {
		delete[] cOutCopyFaceData.m_ppLoops;
		return false;
	}

	nResult = A3DTopoFaceCreate(&cOutCopyFaceData, &pcOutCopyFace);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	return true;
}

bool H3DX::SimplifierImpl::CopyLoop(const A3DTopoLoop * pcInLoop, A3DTopoLoop *& pcOutCopyLoop)
{
	if (nullptr == pcInLoop) {
		return false;
	}

	A3DTopoLoopData cInLoopData;
	A3D_INITIALIZE_DATA(A3DTopoLoopData, cInLoopData);
	A3DStatus nResult = A3DTopoLoopGet(pcInLoop, &cInLoopData);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	nResult = A3DTopoLoopCreate(&cInLoopData, &pcOutCopyLoop);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

/*
	A3DTopoLoopData cOutCopyLoopData;
	A3D_INITIALIZE_DATA(A3DTopoLoopData, cOutCopyLoopData);

	cOutCopyLoopData.m_ucOrientationWithSurface = cInLoopData.m_ucOrientationWithSurface;
	cOutCopyLoopData.m_uiCoEdgeSize = cInLoopData.m_uiCoEdgeSize;
	cOutCopyLoopData.m_ppCoEdges = new A3DTopoCoEdge * [cOutCopyLoopData.m_uiCoEdgeSize];

	for (A3DUns32 nIndex = 0; nIndex < cOutCopyLoopData.m_uiCoEdgeSize; nIndex++) {
		if (true == CopyCoEdge(cInLoopData.m_ppCoEdges[nIndex], cOutCopyLoopData.m_ppCoEdges[nIndex])) {
			delete[] cOutCopyLoopData.m_ppCoEdges;
			return false;
		}
	}

	nResult = A3DTopoLoopCreate(&cOutCopyLoopData, &pcOutCopyLoop);
	if (A3D_SUCCESS != nResult) {
		return false;
	}
*/

	return true;
}

bool H3DX::SimplifierImpl::CopyCoEdge(const A3DTopoCoEdge * pcInCoEdge, A3DTopoCoEdge *& pcOutCopyCoEdge)
{
	if (nullptr == pcInCoEdge) {
		return false;
	}

	A3DTopoCoEdgeData cInCoEdgeData;
	A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, cInCoEdgeData);
	A3DStatus nResult = A3DTopoCoEdgeGet(pcInCoEdge, &cInCoEdgeData);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	A3DTopoCoEdgeData cOutCopyCoEdgeData;
	A3D_INITIALIZE_DATA(A3DTopoCoEdgeData, cOutCopyCoEdgeData);

	cOutCopyCoEdgeData.m_ucOrientationWithLoop = cInCoEdgeData.m_ucOrientationWithLoop;
	cOutCopyCoEdgeData.m_ucOrientationUVWithLoop = cInCoEdgeData.m_ucOrientationUVWithLoop;

	if (false == CopyEdge(cInCoEdgeData.m_pEdge, cOutCopyCoEdgeData.m_pEdge)) {
		return false;
	}

	cOutCopyCoEdgeData.m_pUVCurve = cInCoEdgeData.m_pUVCurve; // 일단 저장되어 있는 UVCurve를 그대로 사용합니다.

	nResult = A3DTopoCoEdgeCreate(&cOutCopyCoEdgeData, &pcOutCopyCoEdge);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	return true;
}

bool H3DX::SimplifierImpl::CopyEdge(const A3DTopoEdge * pcInEdge, A3DTopoEdge *& pcOutCopyEdge)
{
	if (nullptr == pcInEdge) {
		return false;
	}

	A3DTopoEdgeData cInEdgeData;
	A3D_INITIALIZE_DATA(A3DTopoEdgeData, cInEdgeData);
	A3DStatus nResult = A3DTopoEdgeGet(pcInEdge, &cInEdgeData);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	A3DTopoEdgeData cOutCopyEdgeData;
	A3D_INITIALIZE_DATA(A3DTopoEdgeData, cOutCopyEdgeData);

	CopyVertex(cInEdgeData.m_pStartVertex, cOutCopyEdgeData.m_pStartVertex);
	CopyVertex(cInEdgeData.m_pEndVertex, cOutCopyEdgeData.m_pEndVertex);

	cOutCopyEdgeData.m_p3dCurve = cInEdgeData.m_p3dCurve; // 일단 기존 Curve값을 그냥 사용한다.

	cOutCopyEdgeData.m_bHasTrimDomain = cInEdgeData.m_bHasTrimDomain;

	CopyIntervalData(cInEdgeData.m_sInterval, cOutCopyEdgeData.m_sInterval);

	cOutCopyEdgeData.m_dTolerance = cInEdgeData.m_dTolerance;

	nResult = A3DTopoEdgeCreate(&cOutCopyEdgeData, &pcOutCopyEdge);
	if (A3D_SUCCESS != nResult) {
		return false;
	}

	return true;
}

bool H3DX::SimplifierImpl::CopyVertex(const A3DTopoVertex * pcInVertex, A3DTopoVertex *& pcOutCopyVertex)
{
	if (nullptr == pcInVertex) {
		return false;
	}

	A3DEEntityType eType = kA3DTypeUnknown;
	A3DEntityGetType(pcInVertex, &eType);

	bool bResult = false;

	switch (eType)
	{
		case kA3DTypeTopoUniqueVertex: {
			A3DTopoUniqueVertexData cInUniqueVertexData;
			A3D_INITIALIZE_DATA(A3DTopoUniqueVertexData, cInUniqueVertexData);

			if (A3D_SUCCESS != A3DTopoUniqueVertexGet(pcInVertex, &cInUniqueVertexData)) {
				return false;
			}

			if (A3D_SUCCESS == A3DTopoUniqueVertexCreate(&cInUniqueVertexData, &pcOutCopyVertex)) {
				bResult = true;
			}

			A3DTopoUniqueVertexGet(nullptr, &cInUniqueVertexData);
		} break;

		case kA3DTypeTopoMultipleVertex: {
			A3DTopoMultipleVertexData cInMultipleVertexData;
			A3D_INITIALIZE_DATA(A3DTopoMultipleVertexData, cInMultipleVertexData);

			if (A3D_SUCCESS != A3DTopoMultipleVertexGet(pcInVertex, &cInMultipleVertexData)) {
				return false;
			}

			if (A3D_SUCCESS == A3DTopoMultipleVertexCreate(&cInMultipleVertexData, &pcOutCopyVertex)) {
				bResult = true;
			}

			A3DTopoMultipleVertexGet(nullptr, &cInMultipleVertexData);
		} break;
	}

	return bResult;
}


bool H3DX::SimplifierImpl::CopyIntervalData(const A3DIntervalData & cInIntervalData, A3DIntervalData & cOutCopyIntervalData)
{
	A3D_INITIALIZE_DATA(A3DIntervalData, cOutCopyIntervalData);

	cOutCopyIntervalData.m_dMin = cInIntervalData.m_dMin;
	cOutCopyIntervalData.m_dMax = cInIntervalData.m_dMax;

	return true;
}


//== 정보 수집 함수 =================================================================================
bool H3DX::SimplifierImpl::GetBrepDataData(const A3DTopoBrepData * pcInBrepData, A3DTopoBrepDataData & cOutBrepDataData)
{
	A3D_INITIALIZE_DATA(A3DTopoBrepDataData, cOutBrepDataData);
	A3DStatus nResult = A3DTopoBrepDataGet(pcInBrepData, &cOutBrepDataData);

	if (A3D_SUCCESS != nResult) {
		return false;
	}

	return true;
}
