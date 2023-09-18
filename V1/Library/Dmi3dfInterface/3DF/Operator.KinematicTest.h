#pragma once

#include "3DF.h"
#include "Math.h"

#include "Selection.h"
#include "Segment.h"

#include <HOpCameraOrbit.h>

namespace H3DF
{
	namespace Operator
	{
		class KinematicTest
		{
		public:
			bool CreateTestModel(SegmentKey cModelKey);

			bool CreateCylinder(SegmentKey cKey, Point cCenter, float fRadius, float fHeight, Vector cXAxis, Vector cZAxis, int nSidesCount = 36);
		};
	}
}