#pragma once

#include "3DF.h"
#include "Key.h"

namespace H3DF
{
	class API_3DF Kit : public Object
	{
	public:
		Kit();

		H3DF::Type ObjectType() const { return H3DF::Type::Kit; };
	};
}