#pragma once

#include <cur_line_segment.h>

#include "RtCurve.h"

class RtLineSegment : public MbLineSegment, public RtCurve
{
public:
	RtLineSegment(const MbCartPoint & p1, const MbCartPoint & p2);
};
