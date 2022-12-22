#include "stdafx.h"

#include "DmiOrientedEdge.h"

DmiOrientedEdge::DmiOrientedEdge(const MbCurveEdge & edge, bool orient, bool bUsedCurveOneFlag) :
	MbOrientedEdge(edge, orient)
{
	m_bUsedCurveOneFlag = bUsedCurveOneFlag;
}
