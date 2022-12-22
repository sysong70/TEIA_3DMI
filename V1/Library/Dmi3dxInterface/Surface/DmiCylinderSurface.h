#pragma once

#include "DmiSurface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <surf_cylinder_surface.h>

class DmiCylinderSurface : public DmiSurface, public MbCylinderSurface
{
public:
	DmiCylinderSurface(const MbPlacement3D & place, double r, double h);

	void CheckPoint(MbCartPoint & cPoint) const override;
};

