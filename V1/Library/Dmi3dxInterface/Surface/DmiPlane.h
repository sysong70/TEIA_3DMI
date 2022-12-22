#pragma once

#include "DmiSurface.h"

#include <templ_p_array.h>
#include <templ_s_array_rw.h>

#include <surf_plane.h>

class DmiPlane : public DmiSurface, public MbPlane
{
public:
	DmiPlane(const MbCartPoint3D & c0, const MbVector3D & ax, const MbVector3D & ay);
};