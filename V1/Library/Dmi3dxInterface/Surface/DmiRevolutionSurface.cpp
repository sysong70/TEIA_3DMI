#include "stdafx.h"
#include "DmiRevolutionSurface.h"

DmiRevolutionSurface::DmiRevolutionSurface(const MbCurve3D & c, const MbAxis3D & a, double anMin, double anMax, bool same) :
    MbRevolutionSurface(c, a, anMin, anMax, same)
{

}

DmiRevolutionSurface::DmiRevolutionSurface(const MbCurve3D & c, const MbCartPoint3D & p, const MbVector3D & a, double angle, bool same) :
    MbRevolutionSurface(c, p, a, angle, same)
{
}

void DmiRevolutionSurface::CheckPoint(MbCartPoint & cPoint) const
{
	cPoint.x = cPoint.x * m_dUCoeffA;
	cPoint.y = cPoint.y * m_dVCoeffA;
}

