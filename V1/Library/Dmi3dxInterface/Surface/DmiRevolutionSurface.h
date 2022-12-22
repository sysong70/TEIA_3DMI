#pragma once

#include "DmiSurface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <surf_revolution_surface.h>

class DmiRevolutionSurface : public DmiSurface, public MbRevolutionSurface
{
public:
	DmiRevolutionSurface(const MbCurve3D & c, const MbAxis3D & a, double anMin, double anMax, bool same);
	DmiRevolutionSurface(const MbCurve3D & c, const MbCartPoint3D & p, const MbVector3D & a, double angle, bool same);

	void CheckPoint(MbCartPoint & cPoint) const override;
};