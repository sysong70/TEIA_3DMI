#pragma once

#include "3DF.h"
#include "Key.h"

namespace H3DF
{
	class API_3DF Kit : public Object
	{
	public:
		Kit();

		static const H3DF::Type staticType = H3DF::Type::Kit;
		H3DF::Type ObjectType() const override { return staticType; };
	};
}