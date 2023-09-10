#pragma once

#include "3DF.h"

OPEN_3DF_NAMESPACE

class API_3DF BoundingKit
{
public:
	BoundingKit() {}

	BoundingKit & SetExclusion(bool bInExclude);
	bool ShowExclusion(bool & bOutEexclusion) const; 

private:
	bool m_bExclude = false;
};

CLOSE_3DF_NAMESPACE