#pragma once

#include "ConvObject.h"

#include <topology.h>

#include <ppl.h>
#include <vector>

class ConvCoEdge;

using ConvCoEdgeVector = std::vector<ConvCoEdge *>;
using OrientedEdgeList = std::list<const MbOrientedEdge *>;

class ConvLoop : public ConvObject
{
public:
	ConvLoop(A3DTopoLoop * pcTopoLoop, A3DSurfBase * pcSurfBase, double dContextScale);
	ConvLoop(A3DTopoLoop * pcTopoLoop, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap & mpcConvSurfaceMap);
	~ConvLoop();

	bool Convert(ConvSurface * pcConvSurface, bool bOrientationWithShell, bool bFirstLoopFlag);

	// ConvCoEdge 정보 수집
	void CollectConvCoEdge(ConvCoEdgeMap & mpcConvCoEdge);
	
	// TopoVertex 정보 수집
	void CollectTopoVertex(ConvVertexMap & mpcConvVertexMap);

	A3DTopoLoopData & GetTopoLoopData() { return m_cTopoLoopData; }

	c3d::LoopSPtr GetMbLoop() { return m_pcLoop; }
	void SetMbLoop(MbLoop * pcLoop) { m_pcLoop = pcLoop; }

	ConvCoEdgeVector & GetConvCoEdgeVector() { return m_vpcConvCoEdgeVector; }
protected:
	bool GetCoEdgeData(A3DTopoCoEdge * pcTopoCoEdge, A3DSurfBase * pcSurfBase, double dContextScale);
	bool GetCoEdgeData(A3DTopoCoEdge * pcTopoCoEdge, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap & mpcConvSurfaceMap);

	// OrientedEdge의 Open 여부를 확인하고 Open된 경우 Close되도록 Edge를 추가한다.
	void CheckOrientedEdgeOpenAndHealing(OrientedEdgeList & lpcOrientedEdgeList, const MbSurface * pcBaseSurface, double dTolerance);
	// Oreient Edge가 연결되어 있는지 여부를 확인한다. 연결되어 있지 않은 경우 2개의 포인트를 찾아옴.
	bool CheckConectivity(const MbOrientedEdge * pcCurEdge, const MbOrientedEdge * pcNextEdge,
		const MbSurface * pcBaseSurface, double dTolerance, bool & bConnectFlag, const MbCurve ** pcConectiCurve = nullptr);
	// 주어진 두 점을 이용해서 Line Segment를 Base로하는 Oriented Edge를 생성.
	// 주어진 Surface를 One, Two로 하는 Oriented Edge를 생성함.
	bool CreateLineSegmentOrientedEdge(OrientedEdgeList & lpcOrientedEdgeList,
		const MbOrientedEdge * pcCurEdge, const MbOrientedEdge * pcNextEdge, const MbSurface * pcBaseSurface, double dTolerance,
		const MbOrientedEdge *& pcLineSegmentOrientedEdge);

	bool IsOrientedEdgeAllPlaneLine(OrientedEdgeList & lpcOrientedEdgeList, const MbSurface * pcBaseSurface);

private:
	A3DTopoLoopData m_cTopoLoopData;

	ConvCoEdgeVector m_vpcConvCoEdgeVector;

	c3d::LoopSPtr m_pcLoop;
};