#pragma once

#include "DmiSurface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <surf_sphere_surface.h>

class DmiSphereSurface : public DmiSurface, public MbSphereSurface
{
public:
	DmiSphereSurface(const MbPlacement3D & pl, double r);

	void CheckPoint(MbCartPoint & cPoint) const override;
};