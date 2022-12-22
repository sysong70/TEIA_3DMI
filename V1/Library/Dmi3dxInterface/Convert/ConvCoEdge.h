#pragma once

#include "ConvObject.h"

#include <curve.h>

class ConvEdge;
class ConvSurface;
class ConvCurve;

class MbOrientedEdge;

class ConvCoEdge : public ConvObject
{
public:
	ConvCoEdge(A3DTopoCoEdge * pcTopoCoEdge, A3DSurfBase * pcSurfBase, double dContextScale);
	ConvCoEdge(A3DTopoCoEdge * pcTopoCoEdge, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap & mpcConvSurfaceMap);
	~ConvCoEdge();

	const A3DSurfBase * GetSurfBase() { return m_pcSurfBase; }

	c3d::SurfaceSPtr & GetSurface() { return m_pcSurface; }
	void SetSurface(c3d::SurfaceSPtr & pcSurface) { m_pcSurface = pcSurface; }
	
	c3d::PlaneCurveSPtr & GetPlaneCurve() { return m_pcPlaneCurve; }

	bool Convert();

	// Plane Curve를 변환
	bool ConvertCurve(double dContextScale);

	// TopoVertex 정보 수집
	void CollectTopoVertex(ConvVertexMap & mpcConvVertexMap);

	c3d::SurfaceSPtr m_pcSurface;

private:
	const A3DSurfBase * m_pcSurfBase = nullptr;

	c3d::PlaneCurveSPtr m_pcPlaneCurve;

public:
	ConvEdge * GetConvEdge() { return m_pcConvEdge; }
	A3DTopoCoEdge * GetTopoCoEdge() { return m_pcTopoCoEdge; }
	A3DTopoCoEdgeData & GetTopoCoEdgeData() { return m_cTopoCoEdgeData; }

	ConvCurve * GetConvCurve() { return m_pcCurve; }

	ConvSurface * GetNeighborSurface() { return m_pcNeighborSurface; }
	ConvCurve * GetNeighborCurve() { return m_pcNeighborCurve; }

	A3DTopoCoEdge * GetNeighborTopoCoEdge() { return m_pcNeighborTopoCoEdge; }
	void SetNeighborConvCoEdge(ConvCoEdge * pcNeighborConvCoEdge) { m_pcNeighborConvCoEdge = pcNeighborConvCoEdge; }

	MbOrientedEdge * GetOrientedEdge() { return m_pcOrientedEdge; }
	void SetOrientedEdge(MbOrientedEdge * pcOrientedEdge) { m_pcOrientedEdge = pcOrientedEdge; }

	// Thread 변환용 변수
	ConvSurfaceMap * m_mpcConvSurfaceMap = nullptr;
	SurfBaseMap * m_mpcSurfBaseMap = nullptr;
	ConvCoEdgeMap * m_mpcConvCoEdgeMap = nullptr;
	VertexMap * m_mpcMbVertexMap = nullptr;

protected:
	bool GetEdgeData(A3DTopoEdge * pcTopoEdge);

	// ----- Curve 정보 수집 -----
protected:
	bool GetCurveData(A3DCrvBase * pcCrvBase, double dContextScale, ConvCurve *& pcCurve);

	// ----- Neighbor CoEdge 관련 정보 -----
protected:
	bool GetNeighborSurface(A3DTopoCoEdge * pcTopoCoEdge, double dContextScale, const A3DTopoBrepData * pcBrepData, ConvSurfaceMap mpcConvSurfaceMap);

private:
	ConvSurface * m_pcNeighborSurface = nullptr;
	ConvCurve * m_pcNeighborCurve = nullptr;

private:
	A3DTopoCoEdge * m_pcTopoCoEdge = nullptr;
	A3DTopoCoEdgeData m_cTopoCoEdgeData;

	A3DTopoCoEdge * m_pcNeighborTopoCoEdge = nullptr;
	ConvCoEdge * m_pcNeighborConvCoEdge = nullptr;

	MbOrientedEdge * m_pcOrientedEdge = nullptr;

	ConvEdge * m_pcConvEdge = nullptr;
	c3d::EdgeSPtr m_pcCurveEdge;

	ConvCurve * m_pcCurve = nullptr;

	// == Uility 함수 ===============================================================================
protected:
	bool GetSurfBaseFromTopoCoEdge(const A3DTopoCoEdge * psCoEdge, const A3DTopoBrepData * pcBrepData, const A3DSurfBase *& pcSurfBase);
	bool GetTopoFaceFromTopoCoEdge(const A3DTopoCoEdge * psCoEdge, const A3DTopoBrepData * pcBrepData, const A3DTopoFace *& pcTopoFace);
	bool GetSurfBaseFromTopoFace(const A3DTopoFace * pcTopoFace, const A3DSurfBase *& pcSurfBase);
};