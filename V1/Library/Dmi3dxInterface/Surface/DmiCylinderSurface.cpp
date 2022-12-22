#include "stdafx.h"

#include "DmiCylinderSurface.h"

DmiCylinderSurface::DmiCylinderSurface(const MbPlacement3D & place, double r, double h) :
	MbCylinderSurface(place, r, h)
{
}

void DmiCylinderSurface::CheckPoint(MbCartPoint & cPoint) const
{
	cPoint.x = cPoint.x * m_dUCoeffA;
	//cPoint.y = cPoint.y * m_dVCoeffA;
}