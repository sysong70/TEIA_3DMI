#pragma once

#include "DmiSurface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <surf_cone_surface.h>

class DmiConeSurface : public DmiSurface, public MbConeSurface
{
public:
	DmiConeSurface(const MbPlacement3D & pl, double r, double a, double h);
	DmiConeSurface(double r, double a, double h, const MbPlacement3D & pl, double v1, double v2);

	void CheckPoint(MbCartPoint & cPoint) const override;

// 	bool IsNegativeAngle() { return m_bNegativeAngleFlag; }
// 	void SetNegativeAngle(bool bFlag) { m_bNegativeAngleFlag = bFlag; }

	void SetNegativeRadius(bool bFlag) { m_bNegativeRadiusFlag = bFlag; }

	void SetVCoeff1(double dVCoeff1) { m_dVCoeff1 = dVCoeff1; }

private:
	double m_dVCoeff1 = 0.0;
	bool m_bNegativeRadiusFlag = false;
//	bool m_bNegativeAngleFlag = false;
};