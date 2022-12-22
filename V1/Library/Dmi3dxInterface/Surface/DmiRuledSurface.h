#pragma once

#include "DmiSurface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <surf_ruled_surface.h>

class DmiRuledSurface : public DmiSurface, public MbRuledSurface
{
public:
	DmiRuledSurface(const MbCurve3D & c1, const MbCurve3D & c2, bool same);

	void CheckPoint(MbCartPoint & cPoint) const override;
};