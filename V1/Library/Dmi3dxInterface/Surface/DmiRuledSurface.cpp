#include "stdafx.h"
#include "DmiRuledSurface.h"

DmiRuledSurface::DmiRuledSurface(const MbCurve3D & c1, const MbCurve3D & c2, bool same) :
	MbRuledSurface(c1, c2, same)
{
}

void DmiRuledSurface::CheckPoint(MbCartPoint & cPoint) const
{
	cPoint.x = cPoint.x * m_dUCoeffA;
	cPoint.y = cPoint.y * m_dVCoeffA;
}

