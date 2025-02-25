#include "StdAfx.h"

#include "Database.h"

#include "Segment.h"
#include "Portfolio.h"

#include <hc.h>

using namespace H3DF;

SegmentKey H3DF::Database::CreateRootSegment()
{
	HC_KEY nKey = INVALID_KEY;
	HC_Open_Segment("/"); {
		nKey = HC_Create_Segment("");
	} HC_Close_Segment();

	SegmentKey cSegment(nKey);
	return cSegment;
}

void H3DF::Database::RelinquishMemory()
{
	HC_Relinquish_Memory();
}