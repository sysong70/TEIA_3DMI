#pragma once

#include "../3DF/3DF.h"
#include "../3DF/Key.h"

namespace H3DF
{
	class API_3DF Sprocket : public Object
	{
	public:
		Sprocket();

		H3DF::Type ObjectType() const { return H3DF::Type::Sprocket; };
	};
}