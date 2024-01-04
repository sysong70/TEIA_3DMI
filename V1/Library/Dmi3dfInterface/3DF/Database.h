#pragma once

#include "3DF.h"

namespace H3DF
{
	class Database
	{
	public:
		Database() {};

		static SegmentKey CreateRootSegment();

		static PortfolioKey CreatePortfolio();

		static void RelinquishMemory();
	};
}