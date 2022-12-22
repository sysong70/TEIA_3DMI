#pragma once

#ifndef INITIALIZE_A3D_API
	#include <A3DSDKIncludes.h>
#endif

// ----- C3D Header -----
#include <templ_array2.h> // 앞쪽에 추가되어야 함.
#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <topology.h>

#include <unordered_map>

#include <cassert>
#define ASSERT	assert

#include <atlcoll.h>

class MbItem;
class MbSolid;
class MbSurface;
class MbCurveEdge;
class MbVertex;

class ConvSurface;
class ConvCoEdge;
class ConvVertex;

struct ConvEntityInfo
{
	int nEntityCount = 0;
	int nTargetEntityIndex = -1; // -1이 아닌 경우는 초기화해서 사용한다.
	A3DEEntityType * peEntityType = nullptr;
	DWORD_PTR ** pcEntityData = nullptr;
	const DWORD_PTR * pcC3dEntity = nullptr;

	ConvEntityInfo() {}
	ConvEntityInfo(int nCount);
	~ConvEntityInfo() { Delete(); }
	bool Init(int nCount);
	void Delete();
	void ClearData();
	int SetTargetEntityIndex(int nIndex)
	{
		nTargetEntityIndex = nIndex;
		return nIndex;
	}
};

using SolidMap = CAtlMap<DWORD_PTR, MbSolid *>;
using MeshMap = CAtlMap<DWORD_PTR, MbMesh *>;
using ConvSurfaceMap = CAtlMap<DWORD_PTR, ConvSurface *>;
using SurfBaseEntInfoMap = std::unordered_map<const A3DSurfBase *, ConvEntityInfo *>;
using SurfBaseMap = CAtlMap<DWORD_PTR, c3d::SurfaceSPtr>;
using ConvCoEdgeMap = CAtlMap<DWORD_PTR, ConvCoEdge *>;
using VertexMap = CAtlMap<DWORD_PTR, c3d::VertexSPtr>;
using ConvVertexMap = CAtlMap<DWORD_PTR, ConvVertex *>;

class ConvObject
{
public:
	ConvObject();
	~ConvObject();

	bool IsInit() { return m_bInitFlag; }

protected:
	bool GetConvSurfaceData(const A3DSurfBase * pcSurfBase, double dContextScale, ConvSurfaceMap & mpcConvSurfaceMap, ConvSurface *& pcSurface);

protected:
	bool m_bInitFlag = false;
};