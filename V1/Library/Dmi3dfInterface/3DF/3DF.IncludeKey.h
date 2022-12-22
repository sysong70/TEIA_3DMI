#pragma once

#include "3DF.Key.h"

OPEN_3DF_NAMESPACE

class API_3DF IncludeKey : public Key
{
public:
	IncludeKey();
	IncludeKey(HC_KEY nKey);
	IncludeKey(IncludeKey const & cInThat);

	IncludeKey & operator = (IncludeKey const & cOther);
};

CLOSE_3DF_NAMESPACE