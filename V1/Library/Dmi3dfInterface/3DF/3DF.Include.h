#pragma once

#include "3DF.Key.h"

OPEN_3DF_NAMESPACE

class API_3DF IncludeKey : public Key
{
public:
	IncludeKey(HC_KEY nInKey = INVALID_KEY);
	IncludeKey(IncludeKey const & cInThat);

	void Set(IncludeKey const & cInThat);
	IncludeKey & operator = (IncludeKey const & cInThat);
};

CLOSE_3DF_NAMESPACE