#pragma once

#include "3DF.h"
#include "3DF.Key.h"

OPEN_3DF_NAMESPACE

class API_3DF LineKey : public Key
{
public:
	LineKey() {} ;
	LineKey(HC_KEY nInKey);
};

CLOSE_3DF_NAMESPACE