#pragma once

#include "ConvObject.h"

#include <ppl.h>
#include <concurrent_vector.h>

class ConvCurve;
class ConvVertex;

class ConvEdge : public ConvObject
{
public:
	ConvEdge(A3DTopoEdge * pcTopoEdge);
	~ConvEdge();

	A3DTopoVertex * GetStartTopoVertex();
	A3DTopoVertex * GetEndTopoVertex();

	void SetStartConvVertex(ConvVertex * pcConvVertex) { m_pcStartConvVertex = pcConvVertex; }
	void SetEndConvVertex(ConvVertex * pcConvVertex) { m_pcEndConvVertex = pcConvVertex; }

	ConvVertex * GetStartConvVertex() { return m_pcStartConvVertex; }
	ConvVertex * GetEndConvVertex() { return m_pcEndConvVertex; }

	// TopoVertex 정보 수집
	void CollectTopoVertex(ConvVertexMap & mpcConvVertexMap);

private:
	A3DTopoEdgeData m_cTopoEdgeData;
 
	// ----- Vertext 정보 수집 -----
protected:
	bool GetVertexData(A3DTopoVertex * pcTopoVertex, ConvVertex *& pcVertex);

private:
	ConvVertex * m_pcStartConvVertex = nullptr;
	ConvVertex * m_pcEndConvVertex = nullptr;
};