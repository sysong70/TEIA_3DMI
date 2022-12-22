#pragma once

#include "ConvObject.h"

#include <ppl.h>
#include <vector>

class ConvLoop;
class ConvSurface;

using ConvLoopVector = std::vector<ConvLoop *>;

class ConvFace : public ConvObject
{
public:
	ConvFace(A3DTopoFace * pcTopoFace, bool bOrientationWithShell, double dContextScale);
	ConvFace(A3DTopoFace * pcTopoFace, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap & mpcConvSurfaceMap);
	~ConvFace();

	bool Convert(const ConvSurfaceMap & mpcConvSurfaceAtlMap);
	bool ConvertMap(const ConvSurfaceMap & mpcConvSurfaceMap);

	c3d::FaceSPtr & GetFace() { return m_pcFace; }

protected:
	bool ConvertLoop();

public:
	void CollectConvSurface(ConvSurfaceMap & pcSurfBaseMap);
	void CollectConvCoEdge(ConvCoEdgeMap & mpcConvCoEdge);
	void CollectTopoVertex(ConvVertexMap & mpcConvVertexMap);

	A3DTopoFace * GetTopoFace() { return m_pcTopoFace; }
	A3DTopoFaceData & GetTopoFaceData() { return m_cTopoFaceData; }

	ConvSurface * GetConvSurface() { return m_pcConvSurface; }

	ConvLoopVector & GetLoopVector() { return m_vpcConvLoopVector; }

protected:
	bool GetConvSurfaceData(const A3DSurfBase * pcSurfBase, double dContextScale, ConvSurface *& pcSurface);

	bool GetLoopData(A3DTopoLoop * pcTopoLoop, A3DSurfBase * pcSurfBase, double dContextScale);
	bool GetLoopData(A3DTopoLoop * pcTopoLoop, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap mpcConvSurfaceMap);
	bool GetCoEdgeData(A3DTopoCoEdge * pcTopoCoEdge);

	// == Utility 함수 ==============================================================================
	bool CreateBoundedSurface(c3d::SurfaceSPtr & pcSurface, c3d::LoopSPtr & pcLoop, bool bOrientationWithShell, c3d::SurfaceSPtr & cBoundedSurface);

private:
	ConvEntityInfo m_cSurfaceInfo;

	c3d::FaceSPtr m_pcFace;

	A3DTopoFace * m_pcTopoFace = nullptr;
	A3DTopoFaceData m_cTopoFaceData;
	ConvSurface * m_pcConvSurface = nullptr;

	ConvLoopVector m_vpcConvLoopVector;
	c3d::LoopsSPtrVector m_vpcLoopVector;

	bool m_bOrientationWithShell = true;
};