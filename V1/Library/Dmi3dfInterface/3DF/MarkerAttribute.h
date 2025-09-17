#pragma once

#include "3DF.h"
#include "Control.h"

#include "Marker.h"

namespace H3DF
{
	class API_3DF MarkerAttributeControl : public Control
	{
	public:
		MarkerAttributeControl(SegmentKey cInSegment);

		static const H3DF::Type staticType = H3DF::Type::MarkerAttributeControl;
		H3DF::Type ObjectType() const { return staticType; };

		MarkerAttributeControl const & operator = (MarkerAttributeControl const & cInThat);

		MarkerAttributeControl & SetSize(float fInSize, Marker::SizeUnits nInUnits = Marker::SizeUnits::ScaleFactor);
	};
}
