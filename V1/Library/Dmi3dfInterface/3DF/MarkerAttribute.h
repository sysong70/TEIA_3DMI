#pragma once

#include "3DF.h"
#include "Control.h"

#include "Marker.h"

namespace H3DF
{
	class API_3DF MarkerAttributeControl : public Control
	{
	public:
		MarkerAttributeControl(SegmentKey nInKey);
		MarkerAttributeControl & SetSize(float fInSize, Marker::SizeUnits nInUnits = Marker::SizeUnits::ScaleFactor);
	};
}
