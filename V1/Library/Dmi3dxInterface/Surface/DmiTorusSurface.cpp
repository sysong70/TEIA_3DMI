#include "stdafx.h"
#include "DmiTorusSurface.h"

DmiTorusSurface::DmiTorusSurface(const MbPlacement3D & pl, double initMajorR, double initMinorR):
	MbTorusSurface(pl, initMajorR, initMinorR)
{
}

DmiTorusSurface::DmiTorusSurface(double initMajorR, double initMinorR, const MbPlacement3D & pl, double vin, double vax):
    MbTorusSurface(initMajorR, initMinorR, pl, vin, vax)
{

}

void DmiTorusSurface::CheckPoint(MbCartPoint & cPoint) const
{
	cPoint.x = cPoint.x * m_dUCoeffA + m_dUCoeffB;
	cPoint.y = cPoint.y * m_dVCoeffA + m_dVCoeffB;

	if(GetMajorRadius() < GetMinorRadius()) {
		double dAngle = M_PI - acos(GetMajorRadius() / GetMinorRadius());
		if(dAngle < cPoint.y) {
			cPoint.y -= M_PI2;
		}
	}
}

