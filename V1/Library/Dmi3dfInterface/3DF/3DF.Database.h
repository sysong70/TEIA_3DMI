#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

class Database
{
public:
	Database() {};

	static SegmentKey CreateRootSegment();

	static PortfolioKey CreatePortfolio();

	static void RelinquishMemory();
};

CLOSE_3DF_NAMESPACE