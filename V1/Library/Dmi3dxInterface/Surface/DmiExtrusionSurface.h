#pragma once

#include "DmiSurface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <surf_extrusion_surface.h>

class DmiExtrusionSurface : public DmiSurface, public MbExtrusionSurface
{
public:
	DmiExtrusionSurface(const MbCurve3D & curve, const MbVector3D & vector, bool same);

	void CheckPoint(MbCartPoint & cPoint) const override;
};