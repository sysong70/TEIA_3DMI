#pragma once

#include "ConvObject.h"

#include <ppl.h>
#include <vector>

#include <topology_faceset.h>

class ConvFace;

using ConvFaceVector = std::vector<ConvFace *>;
using A3DSurfBaseVector = std::vector<ConvFace *>;

class ConvFaceShell : public ConvObject
{
public:
	ConvFaceShell(A3DTopoShell * pcTopoShell, double dContextScale);
	~ConvFaceShell();

	ConvFaceVector & GetConvFaceVector() { return m_vpcConvFaceVector; }

	// ConvFaceShell에 들어있는 Surface를 검색해서 변환한다.
	bool CollectAndConvertSurface(double dContextScale);
	bool CollectAndConvertSurfaceMap(double dContextScale);
	
	// ConvCoEdge를 수집해서, MbCurveEdge를 생성하도록 한다.
	bool CollectAndConvertTopoCoEdge(double dContextScale);
	bool CollectAndConvertTopoCoEdgeMap(double dContextScale);

	// TopoVertex를 수집해서, MbVertex를 생성하도록 한다.
	bool CollectAndConvertTopoVertex(double dContextScale);
	bool CollectAndConvertTopoVertexMap(double dContextScale);

	//전체적인 Convert를 실시한다.
	bool Convert();
	bool ConvertMap();

	A3DTopoShellData m_cTopoShellData;

private:
	ConvFaceVector m_vpcConvFaceVector;
	
	ConvSurfaceMap m_mpcConvSurfaceMap;
	ConvCoEdgeMap m_mpcConvCoEdgeMap;
	ConvVertexMap m_mpcConvVertexMap;

	double m_dContextScale = 1.0;
};