#pragma once

#include "ConvObject.h"

#include <ppl.h>

class ConvVertex : public ConvObject
{
public:
	ConvVertex(A3DTopoVertex * pcTopoVertex);
	~ConvVertex();

	A3DTopoVertex * GetTopoVertex() { return m_pcTopoVertex; }
	A3DEEntityType GetVertexType() { return m_eVertexType; }
	DWORD_PTR * GetVertexData() { return m_pcVertexData; }

	c3d::VertexSPtr & GetVertex() { return m_pcVertex; }
	void SetVertex(c3d::VertexSPtr & pcVertex) { m_pcVertex = pcVertex; }

	bool Convert(double dContextScale);

	// == 변환 관련 함수 =============================================================================
	bool ConvertTopoVertex(double dContextScale, c3d::VertexSPtr & pcVertex);

protected:
	bool ConvertTopoUniqueVertex(double dContextScale, c3d::VertexSPtr & pcVertex);
	bool ConvertTopoMultipleVertex(double dContextScale, c3d::VertexSPtr & pcVertex);

protected:
	// == Vertex 관련 함수 ===========================================================================
	bool GetTopoUniqueVertex(A3DTopoVertex * pcTopoVertex);
	bool GetTopoMultipleVertex(A3DTopoVertex * pcTopoVertex);

private:
	A3DTopoVertex * m_pcTopoVertex = nullptr;
	A3DEEntityType m_eVertexType = kA3DTypeUnknown;
	DWORD_PTR * m_pcVertexData = nullptr;

	c3d::VertexSPtr m_pcVertex;
};