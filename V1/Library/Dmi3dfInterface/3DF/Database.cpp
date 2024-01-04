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

PortfolioKey H3DF::Database::CreatePortfolio()
{
	HC_KEY nKey = INVALID_KEY;
	HC_KEY nPortfoliosKey = HC_Open_Segment("/portfolios"); {
		HC_Set_Priority(nPortfoliosKey, 0);
		nKey = HC_Create_Segment("");
	} HC_Close_Segment();

	PortfolioKey cPortfolio(nKey);
	return cPortfolio;
}

void H3DF::Database::RelinquishMemory()
{
	HC_Relinquish_Memory();
}