#pragma once

#include "3DF.h"
#include "Object.h"

namespace H3DF
{
	class API_3DF Control : public Object
	{
	protected:
		Control() = default;
		Control(Control && cInThat);
		Control & operator=(Control && cInThat);

		static const H3DF::Type staticType = H3DF::Type::Control;
		H3DF::Type ObjectType() const override { return staticType; }
	};
}
