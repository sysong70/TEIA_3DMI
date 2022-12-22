#include "stdafx.h"

#include "DmiElevationSurface.h"

DmiElevationSurface::DmiElevationSurface(const RPArray<MbCurve3D> & initCurves, bool sameCurves,
	const MbCurve3D & initSpine, bool sameSpine, bool simToEvol) :
    MbElevationSurface(initCurves, sameCurves, initSpine, sameSpine, simToEvol)
{
}

void DmiElevationSurface::CheckPoint(MbCartPoint & cPoint) const
{
    double dTemp = cPoint.x;
    cPoint.x = cPoint.y;
    cPoint.y = dTemp;

	cPoint.x = cPoint.x * m_dUCoeffA;
	cPoint.y = cPoint.y * m_dVCoeffA;
}

