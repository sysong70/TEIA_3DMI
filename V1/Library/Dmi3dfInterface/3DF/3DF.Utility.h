#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

namespace Utility
{
	API_3DF bool ChangeSubSegmentColor(SegmentKey & cInTargetSegment, CString strGeometry, MaterialMappingKit const & cInKit, bool bRecursive = false);

};

CLOSE_3DF_NAMESPACE