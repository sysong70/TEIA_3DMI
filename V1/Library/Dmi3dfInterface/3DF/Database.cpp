#include "StdAfx.h"

#include "Database.h"

#include "Segment.h"
#include "Portfolio.h"

#include <hc.h>

USING_3DF_NAMESPACE

SegmentKey Database::CreateRootSegment()
{
	HC_Open_Segment("/");
		HC_KEY nKey = HC_Create_Segment("");
	HC_Close_Segment();

	SegmentKey cSegment(nKey);
	return cSegment;
}

PortfolioKey Database::CreatePortfolio()
{
	HC_KEY nPortfoliosKey = HC_Open_Segment("/portfolios");
		HC_Set_Priority(nPortfoliosKey, 0);
		HC_KEY nKey = HC_Create_Segment("");
	HC_Close_Segment();

	PortfolioKey cPortfolio(nKey);
	return cPortfolio;
}

void Database::RelinquishMemory()
{
	HC_Relinquish_Memory();
}