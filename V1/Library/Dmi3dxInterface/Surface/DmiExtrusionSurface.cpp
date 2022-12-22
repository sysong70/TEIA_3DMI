#include "stdafx.h"
#include "DmiExtrusionSurface.h"

DmiExtrusionSurface::DmiExtrusionSurface(const MbCurve3D & curve, const MbVector3D & vector, bool same) :
    MbExtrusionSurface(curve, vector, same)
{

}

void DmiExtrusionSurface::CheckPoint(MbCartPoint & cPoint) const
{
	cPoint.x = cPoint.x * m_dUCoeffA;
	cPoint.y = cPoint.y * m_dVCoeffA;
}

