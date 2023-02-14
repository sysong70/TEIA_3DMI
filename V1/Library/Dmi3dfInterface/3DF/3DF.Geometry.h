#pragma once

#include "3DF.h"
#include "3DF.Key.h"

OPEN_3DF_NAMESPACE

class API_3DF GeometryKey : public Key
{
public:
	GeometryKey(HC_KEY nInKey = INVALID_KEY);
};

CLOSE_3DF_NAMESPACE