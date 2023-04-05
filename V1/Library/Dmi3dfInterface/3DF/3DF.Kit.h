#pragma once

#include "3DF.h"
#include "3DF.Key.h"

OPEN_3DF_NAMESPACE

class API_3DF Kit : public Object
{
public:
	Kit();

	TDF::Type ObjectType() const { return TDF::Type::Kit; };
};

CLOSE_3DF_NAMESPACE