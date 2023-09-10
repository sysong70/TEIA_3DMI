#pragma once

#include "3DF.h"
#include "Math.h"

#include "3DF.Selection.h"
#include "Segment.h"

#include <HOpCameraOrbit.h>

OPEN_3DF_NAMESPACE

namespace Operator
{
	class KinematicTest
	{
	public:
		bool CreateTestModel(SegmentKey cModelKey);

		bool CreateCylinder(SegmentKey cKey, Point cCenter, float fRadius, float fHeight, Vector cXAxis, Vector cZAxis, int nSidesCount = 36);
	};
}

CLOSE_3DF_NAMESPACE
