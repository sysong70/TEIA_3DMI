#pragma once

#include "DmiSurface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <surf_torus_surface.h>

class DmiTorusSurface : public DmiSurface, public MbTorusSurface
{
public:
	DmiTorusSurface(const MbPlacement3D & pl, double initMajorR, double initMinorR);
	DmiTorusSurface(double initMajorR, double initMinorR, const MbPlacement3D & pl, double vin, double vax);

	void CheckPoint(MbCartPoint & cPoint) const override;
};