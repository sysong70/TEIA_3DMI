#pragma once

#include "3DF.h"
#include "Object.h"

namespace H3DF
{
	class API_3DF Control : public Object
	{
	public:
		H3DF::Type ObjectType() const { return H3DF::Type::Control; };

	protected:
		Control() = default;
		Control(Control && cInThat);
		Control & operator=(Control && cInThat);
	};
}
