#pragma once

#include "3DF.h"
#include "Control.h"

#include "Marker.h"

namespace H3DF
{
	class API_3DF MarkerAttributeControl : public Control
	{
	public:
		MarkerAttributeControl(SegmentKey cInSegmentKey);

		H3DF::Type ObjectType() const override { return H3DF::Type::MarkerAttributeControl; };

		void Set(MarkerAttributeControl const & cInThat);
		MarkerAttributeControl const & operator = (MarkerAttributeControl const & cInThat);

		MarkerAttributeControl & SetSize(float fInSize, Marker::SizeUnits nInUnits = Marker::SizeUnits::ScaleFactor);
	};
}
