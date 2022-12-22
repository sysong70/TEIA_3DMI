#include "stdafx.h"
#include "DmiSphereSurface.h"

DmiSphereSurface::DmiSphereSurface(const MbPlacement3D & pl, double r):
	MbSphereSurface(pl, r)
{
}

void DmiSphereSurface::CheckPoint(MbCartPoint & cPoint) const
{
	cPoint.x = cPoint.x * m_dUCoeffA;
	cPoint.y = cPoint.y * m_dVCoeffA;
}
