#pragma once

#include "3DF.h"
#include "Key.h"

namespace H3DF
{
	class API_3DF Marker
	{
	public:
		enum class DrawingPreference : uint32_t
		{
			Nicest, // Markers should always be drawn at the requested size at the possible cost of performance.
			Fastest, // Performance should be favored at the possible cost of drawing markers at a smaller size than requested.
		};

		enum class SizeUnits : uint32_t
		{
			ScaleFactor,			// A unitless linear scaling factor.  A value of 2.0 will cause markers to be rendered twice as large.  A value of 1.0 will result in a visually pleasing device-dependent marker size that is approximately 3% of the height of the outer window.  A value of 0 will result in a single pixel marker for display-devices, or the smallest size supported by any other device.
			ObjectSpace,			// Object space units ignoring any scaling components in modelling matrices.
			SubscreenRelative,		// Fraction of the height of the outermost window.
			WindowRelative,			// Fraction of the height of the local window.
			WorldSpace,				// Object space units including any scaling components in modelling matrices and cameras.
			Points,					// Points units typically used for text size.  1 point corresponds to 1/72 inch.
			Pixels					// Number of pixels.
		};

	private:
		Marker() {}
	};

	class API_3DF MarkerKey : public Key
	{
	public:
		MarkerKey() {};
		MarkerKey(HC_KEY nInKey);

		H3DF::Type ObjectType() const override { return H3DF::Type::MarkerKey; };
	};
}