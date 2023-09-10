#pragma once

#include "3DF.h"
#include "Key.h"

OPEN_3DF_NAMESPACE

class API_3DF Kit : public Object
{
public:
	Kit();

	H3DF::Type ObjectType() const { return H3DF::Type::Kit; };
};

CLOSE_3DF_NAMESPACE