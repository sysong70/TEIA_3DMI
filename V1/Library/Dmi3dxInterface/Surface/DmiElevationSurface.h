#pragma once

#include "DmiSurface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <surf_elevation_surface.h>

class DmiElevationSurface : public DmiSurface, public MbElevationSurface
{
public:
	DmiElevationSurface(const RPArray<MbCurve3D> & initCurves, bool sameCurves,
		const MbCurve3D & initSpine, bool sameSpine, bool simToEvol = true);

	void CheckPoint(MbCartPoint & cPoint) const override;
	void SetRail2ParameterV(double dParameter) { m_dRail2ParameterV = dParameter; }

private:
	double m_dRail2ParameterV = 0.0;
};