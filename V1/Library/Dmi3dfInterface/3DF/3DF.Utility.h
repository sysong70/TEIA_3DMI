#pragma once

#include "3DF.h"

#include <chrono>

OPEN_3DF_NAMESPACE

namespace Utility
{
	API_3DF bool ChangeSubSegmentColor(SegmentKey & cInTargetSegment, CString strGeometry, MaterialMappingKit const & cInKit, bool bRecursive = false);
	API_3DF CString GetTimeSpanString(std::chrono::milliseconds & ms);
};

CLOSE_3DF_NAMESPACE