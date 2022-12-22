#include "stdafx.h"
#include "DmiConeSurface.h"

DmiConeSurface::DmiConeSurface(const MbPlacement3D & pl, double r, double a, double h):
	MbConeSurface(pl, r, a, h)
{
}

DmiConeSurface::DmiConeSurface(double r, double a, double h, const MbPlacement3D & pl, double v1, double v2) :
	MbConeSurface(r, a, h, pl, v1, v2), DmiSurface()
{
}

void DmiConeSurface::CheckPoint(MbCartPoint & cPoint) const
{
	cPoint.x = cPoint.x * m_dUCoeffA;
	cPoint.y = cPoint.y * m_dVCoeffA + m_dVCoeff1;

	if(true == m_bNegativeRadiusFlag) {
		cPoint.x = M_PI - cPoint.x;
		cPoint.y = -cPoint.y;
	}
}